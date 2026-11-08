// Copyright Modfest Studios
#include "Subsystems/ServerSubsystem.h"
#include "Online/ServerBrowserSearchResult.h"
#include "Online/ServerSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/NetConnection.h"
#include "Engine/PendingNetGame.h"
#include "GameMapsSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"
#include "Maps/MapAsset.h"
#include "Subsystems/MapSubsystem.h"
#include "../Logging.h"

const FName UServerSubsystem::SERVER_NAME(TEXT("SERVER_NAME"));
const FName UServerSubsystem::GAME_MODE(TEXT("GAME_MODE"));
const FName UServerSubsystem::GAME_VERSION(TEXT("GAME_VERSION"));

namespace
{
// UE selects client travel drivers through a process-wide definition. Lease it only
// during travel, and reject competing subsystem travel instead of changing its transport.
TWeakObjectPtr<UServerSubsystem> TransportOwner;
FNetDriverDefinition SavedGameDriver;
bool bHadGameDriver = false;
}

//========================================================FUNCTIONS========================================================
//========================INITIALIZATION========================
void UServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RequestedSearchSettings = GetDefault<UServerSettings>()->LANSearch;
	RequestedSearchSettings.ConnectionType = EServerConnectionType::LAN;
	bDeinitializing = false;
	if (GEngine)
	{
		NetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &UServerSubsystem::OnNetworkFailure);
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &UServerSubsystem::OnTravelFailure);
	}
	DiscoveryTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UServerSubsystem::TickDiscovery), 0.25f);
	WorldTearDownHandle = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UServerSubsystem::OnWorldTearDown);
}

void UServerSubsystem::Deinitialize()
{
	bDeinitializing = true;
	bLANDiscoveryActive = false;
	bRefreshPending = false;
	FTSTicker::GetCoreTicker().RemoveTicker(DiscoveryTickerHandle);
	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldTearDownHandle);

	// Unbind before cancellation: Null may invoke completion synchronously.
	if (SearchInterface.IsValid())
	{
		SearchInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindDelegateHandle);
		SearchInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(CancelFindDelegateHandle);
		if (SessionSearch.IsValid() && SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
			SearchInterface->CancelFindSessions();
	}

	ReleaseSearch();
	ReleaseTransport();
	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(NetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}
	ClearSessionDelegates();

	if (bOwnsSession && SessionInterface.IsValid() && SessionInterface->GetNamedSession(NAME_GameSession))
		SessionInterface->DestroySession(NAME_GameSession);
	SessionInterface.Reset();
	ServerToJoin = nullptr;
	SearchResults.Empty();

	Super::Deinitialize();
}


//=====================LEGACY SETTINGS=====================
FString UServerSubsystem::GetServerName() const
{
	return ServerName;
}

void UServerSubsystem::SetServerName(const FString& Value)
{
	ServerName = Value;
}

FString UServerSubsystem::GetPassword() const
{
	return Password;
}

void UServerSubsystem::SetPassword(const FString& Value)
{
	Password = Value;
}

FString UServerSubsystem::GetDirectConnectURL() const
{
	return DirectConnectURL;
}

void UServerSubsystem::SetDirectConnectURL(const FString& Value)
{
	DirectConnectURL = Value;
}

//======================
//=======SESSIONS=======
//======================

FSessionInfo UServerSubsystem::GetCurrentSessionInfo()
{
	const FServerSessionInfo Current = GetServerSessionInfo();
	FSessionInfo Info;

	Info.ServerName = Current.ServerName;
	Info.SessionType = StaticEnum<EServerConnectionType>()->GetNameStringByValue(static_cast<int64>(Current.ConnectionType));
	Info.MaxConnections = Current.MaxPlayers;
	Info.OpenConnections = FMath::Max(0, Current.MaxPlayers - Current.CurrentPlayers);
	Info.bIsAdvertising = Current.bPubliclyListed;
	Info.ServerStatus = StaticEnum<EServerState>()->GetNameStringByValue(static_cast<int64>(ServerState));

	return Info;
}

FServerSessionInfo UServerSubsystem::GetServerSessionInfo() const
{
	const FNamedOnlineSession* Session = SessionInterface.IsValid() ? SessionInterface->GetNamedSession(NAME_GameSession) : nullptr;
	FServerSessionInfo Info;

	Info.ConnectionType = ActiveConnectionType;
	if (ServerState != EServerState::Idle && ActiveOperation == EServerOperation::Host)
	{
		Info.ServerName = ActiveHostSettings.ServerName;
		Info.MaxPlayers = ActiveHostSettings.MaxPlayers;
	}
	if (Session)
	{
		Session->SessionSettings.Get(SERVER_NAME, Info.ServerName);
		Info.ConnectionType = Session->SessionSettings.bIsLANMatch ? EServerConnectionType::LAN : EServerConnectionType::Online;
		Info.MaxPlayers = Session->SessionSettings.NumPublicConnections;
		Info.CurrentPlayers = FMath::Max(0, Info.MaxPlayers - Session->NumOpenPublicConnections);
		Info.bPubliclyListed = Session->SessionSettings.bShouldAdvertise;
	}

	return Info;
}

FServerCapabilities UServerSubsystem::GetServerCapabilities() const
{
	FServerCapabilities Capabilities;

	Capabilities.bLANDiscoveryAvailable = ResolveLANInterface().IsValid();
	Capabilities.OnlineProvider = GetDefault<UServerSettings>()->OnlineProvider;
	Capabilities.bOnlineSessionsAvailable = ResolveSessionInterface(EServerConnectionType::Online).IsValid() && IsOnlineUserReady(Capabilities.OnlineProvider);
	Capabilities.LANProvider = Capabilities.bLANDiscoveryAvailable ? FName(TEXT("NULL")) : NAME_None;

	return Capabilities;
}

EServerState UServerSubsystem::GetServerState() const
{
	return ServerState;
}

bool UServerSubsystem::IsSessionValid()
{
	return SessionInterface.IsValid() || ResolveLANInterface().IsValid();
}

//=====================HOSTING=====================
void UServerSubsystem::StartServer(const FServerHostSettings& Settings)
{
	const FGuid RequestId = FGuid::NewGuid();
	UMapSubsystem* Maps = GetGameInstance()->GetSubsystem<UMapSubsystem>();
	const UMapAsset* Map = Maps ? Maps->GetNextMap() : nullptr;
	UWorld* World = GetWorld();
	FOnlineSessionSettings OnlineSettings;
	FString MapURL;
	const FName Provider = Settings.ConnectionType == EServerConnectionType::Online ? GetDefault<UServerSettings>()->OnlineProvider : FName(TEXT("NULL"));

	if (bDeinitializing)
		return;
	if (!World || !Map || Map->Level.IsNull() || Settings.MaxPlayers < 1 || !Password.IsEmpty())
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::InvalidRequest);

		return;
	}
	MapURL = Maps->SantizeMapPath(Map->Level.ToString());
	if (MapURL.IsEmpty())
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::InvalidRequest);

		return;
	}
	if (ServerState == EServerState::Hosting || ServerState == EServerState::Connected)
	{
		// Rehost only after the old provider and network world have both been released.
		QueuedHostSettings = Settings;
		bHostQueued = true;
		bJoinQueued = false;
		BeginLeave(EServerOperation::Leave);

		return;
	}
	if (ServerState != EServerState::Idle || World->GetNetMode() != NM_Standalone)
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::OperationInProgress);

		return;
	}
	if (Settings.ConnectionType == EServerConnectionType::Online && !IsOnlineUserReady(Provider))
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::NotLoggedIn);

		return;
	}
	StopLANDiscovery();
	if (bSearchingForServers || ServerState != EServerState::Idle)
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::OperationInProgress);

		return;
	}
	SessionInterface = ResolveSessionInterface(Settings.ConnectionType, Provider);
	if (Settings.ConnectionType != EServerConnectionType::Direct && !SessionInterface.IsValid())
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::ProviderUnavailable);

		return;
	}
	if (SessionInterface.IsValid() && SessionInterface->GetNamedSession(NAME_GameSession))
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::OperationInProgress);

		return;
	}
	if (!AcquireTransport(Settings.ConnectionType))
	{
		ReportFailure(RequestId, EServerOperation::Host, EServerError::Unsupported);

		return;
	}
	ActiveHostSettings = Settings;
	PendingMapURL = MapURL;
	BeginSessionOperation(RequestId, EServerOperation::Host, Settings.ConnectionType);
	ActiveProvider = Settings.ConnectionType == EServerConnectionType::Direct ? NAME_None : Provider;
	if (Settings.ConnectionType == EServerConnectionType::Direct)
	{
		BeginSessionTravel(PendingMapURL);

		return;
	}
	// Steam listen servers use lobbies. Both flags must match in UE's Steam provider.
	OnlineSettings.bIsLANMatch = Settings.ConnectionType == EServerConnectionType::LAN;
	OnlineSettings.bUsesPresence = Settings.ConnectionType == EServerConnectionType::Online;
	OnlineSettings.bUseLobbiesIfAvailable = OnlineSettings.bUsesPresence;
	OnlineSettings.bAllowJoinViaPresence = OnlineSettings.bUsesPresence;
	OnlineSettings.bShouldAdvertise = Settings.bPubliclyListed;
	OnlineSettings.bAllowJoinInProgress = true;
	OnlineSettings.NumPublicConnections = Settings.MaxPlayers;
	OnlineSettings.Set(SERVER_NAME, Settings.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	CreateDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UServerSubsystem::OnCreateSessionComplete, RequestId));
	SetServerState(EServerState::Creating);
	if (!SessionInterface->CreateSession(0, NAME_GameSession, OnlineSettings) && ServerState == EServerState::Creating)
		OnCreateSessionComplete(NAME_GameSession, false, RequestId);
}

void UServerSubsystem::HostLANSession()
{
	FServerHostSettings Settings;

	Settings.ServerName = ServerName;
	StartServer(Settings);
}

void UServerSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful, FGuid RequestId)
{
	if (bDeinitializing || RequestId != SessionRequestId || SessionName != NAME_GameSession || !CreateDelegateHandle.IsValid())
		return;
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateDelegateHandle);
	CreateDelegateHandle.Reset();
	if (bSessionAbandoned)
	{
		// A timed-out create still owns the provider until this callback arrives.
		DestroyActiveSession();

		return;
	}
	if (!bWasSuccessful || !GetWorld())
	{
		FailSessionOperation(EServerError::ConnectionFailed);

		return;
	}
	BeginSessionTravel(PendingMapURL);
}

//=====================LEAVING & RECOVERY=====================
void UServerSubsystem::LeaveSession()
{
	bHostQueued = false;
	bJoinQueued = false;
	BeginLeave(EServerOperation::Leave);
}

void UServerSubsystem::StopServer()
{
	if (ServerState != EServerState::Hosting && !(ServerState == EServerState::Creating && ActiveOperation == EServerOperation::Host) && ServerState != EServerState::RecoveryFailed)
	{
		ReportFailure(FGuid::NewGuid(), EServerOperation::Stop, EServerError::InvalidRequest);

		return;
	}
	bHostQueued = false;
	bJoinQueued = false;
	BeginLeave(EServerOperation::Stop);
}

void UServerSubsystem::BeginLeave(EServerOperation Operation)
{
	if (bDeinitializing)
		return;
	if (DestroyDelegateHandle.IsValid() || ServerState == EServerState::Leaving || ServerState == EServerState::Recovering)
	{
		ReportFailure(FGuid::NewGuid(), Operation, EServerError::OperationInProgress);

		return;
	}
	StopLANDiscovery();
	if (CreateDelegateHandle.IsValid() || JoinDelegateHandle.IsValid())
	{
		// Create/join cannot be cancelled safely through IOnlineSession. Keep the
		// request identity and destroy the resulting session when its callback arrives.
		bSessionAbandoned = true;
		ReleaseTransport();
		PendingLeaveRequestId = FGuid::NewGuid();
		SetServerState(EServerState::Recovering);
		FinishSessionOperation(EServerError::Cancelled);
		ActiveOperation = Operation;
		bOperationReported = false;

		return;
	}
	SessionRequestId = FGuid::NewGuid();
	ActiveOperation = Operation;
	bOperationReported = false;
	bSessionAbandoned = false;
	OperationStartedAt = FPlatformTime::Seconds();
	SetServerState(EServerState::Leaving);
	DestroyActiveSession();
}

void UServerSubsystem::DestroyActiveSession()
{
	FGuid RequestId;

	// The cancelled create/join callback keeps its original identity. The explicit
	// leave receives its own request ID once that callback releases the provider.
	if (PendingLeaveRequestId.IsValid())
	{
		SessionRequestId = PendingLeaveRequestId;
		PendingLeaveRequestId.Invalidate();
	}
	RequestId = SessionRequestId;

	if (DestroyDelegateHandle.IsValid())
		return;
	OperationStartedAt = FPlatformTime::Seconds();
	SetServerState(EServerState::Leaving);
	if (!bOwnsSession || !SessionInterface.IsValid() || !SessionInterface->GetNamedSession(NAME_GameSession))
	{
		ReturnToMenu();

		return;
	}
	// Do not remove a named session locally: the backend may still own a lobby.
	DestroyDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UServerSubsystem::OnDestroySessionComplete, RequestId));
	if (!SessionInterface->DestroySession(NAME_GameSession) && DestroyDelegateHandle.IsValid())
		OnDestroySessionComplete(NAME_GameSession, false, RequestId);
}

void UServerSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful, FGuid RequestId)
{
	if (bDeinitializing || RequestId != SessionRequestId || SessionName != NAME_GameSession || !DestroyDelegateHandle.IsValid())
		return;
	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroyDelegateHandle);
	DestroyDelegateHandle.Reset();
	if (!bWasSuccessful)
	{
		ReleaseTransport();
		bHostQueued = false;
		bJoinQueued = false;
		SetServerState(EServerState::RecoveryFailed);
		if (bOperationReported)
			ReportFailure(RequestId, ActiveOperation, EServerError::CleanupFailed);
		FinishSessionOperation(EServerError::CleanupFailed);

		return;
	}
	ReturnToMenu();
}

void UServerSubsystem::ReturnToMenu()
{
	const UServerSettings* Settings = GetDefault<UServerSettings>();
	const FString Map = Settings->ReturnMap.IsNull() ? UGameMapsSettings::GetGameDefaultMap() : Settings->ReturnMap.GetLongPackageName();
	UWorld* World = GetWorld();

	ClearSessionDelegates();
	bOwnsSession = false;
	SessionInterface.Reset();
	ReleaseTransport();
	if (!World || Map.IsEmpty())
	{
		bHostQueued = false;
		bJoinQueued = false;
		SetServerState(EServerState::RecoveryFailed);
		FinishSessionOperation(EServerError::CleanupFailed);

		return;
	}
	// Absolute local travel tears down pending/client/listen connections and strips
	// old listen/connection options. Idle is published only after the menu loads.
	TravelSourceWorld = World;
	bReturningToMenu = true;
	OperationStartedAt = FPlatformTime::Seconds();
	SetServerState(EServerState::Leaving);
	UGameplayStatics::OpenLevel(World, FName(*Map), true);
}

void UServerSubsystem::FinishSessionOperation(EServerError Error)
{
	const FServerOperationResult Result = MakeOperationResult(SessionRequestId, ActiveOperation, Error);

	if (bOperationReported)
		return;
	bOperationReported = true;
	if (Error != EServerError::None && Error != EServerError::Cancelled)
		OnServerOperationFailed.Broadcast(Result);
	OnServerOperationCompleted.Broadcast(Result);
}

void UServerSubsystem::FailSessionOperation(EServerError Error)
{
	if (bDeinitializing || ServerState == EServerState::Recovering || ServerState == EServerState::RecoveryFailed || ServerState == EServerState::Leaving)
		return;
	bHostQueued = false;
	bJoinQueued = false;
	bSessionAbandoned = true;
	ReleaseTransport();
	// A connection that was previously established gets a new failure notification.
	if (bOperationReported)
	{
		SessionRequestId = FGuid::NewGuid();
		bOperationReported = false;
	}
	SetServerState(EServerState::Recovering);
	FinishSessionOperation(Error);
	if (!CreateDelegateHandle.IsValid() && !JoinDelegateHandle.IsValid())
		DestroyActiveSession();
}

void UServerSubsystem::OnNetworkFailure(UWorld* World, UNetDriver* Driver, ENetworkFailure::Type Failure, const FString& Message)
{
	// Global engine delegates also report failures from unrelated PIE worlds/beacons.
	if (!World || World->GetGameInstance() != GetGameInstance() || !Driver || (Driver->NetDriverName != NAME_GameNetDriver && Driver->NetDriverName != NAME_PendingNetDriver) || ServerState == EServerState::Idle)
		return;
	UE_LOG(LogServerSubsystem, Warning, TEXT("Session network failure (%d): %s"), static_cast<int32>(Failure), *Message);
	FailSessionOperation(EServerError::ConnectionFailed);
}

void UServerSubsystem::OnTravelFailure(UWorld* World, ETravelFailure::Type Failure, const FString& Message)
{
	if (!World || World->GetGameInstance() != GetGameInstance() || ServerState == EServerState::Idle)
		return;
	UE_LOG(LogServerSubsystem, Warning, TEXT("Session travel failure (%d): %s"), static_cast<int32>(Failure), *Message);
	if (bReturningToMenu)
	{
		bReturningToMenu = false;
		bHostQueued = false;
		bJoinQueued = false;
		SetServerState(EServerState::RecoveryFailed);
		if (bOperationReported)
			ReportFailure(SessionRequestId, ActiveOperation, EServerError::CleanupFailed);
		FinishSessionOperation(EServerError::CleanupFailed);

		return;
	}
	FailSessionOperation(EServerError::ConnectionFailed);
}

//===================================
//=============SEARCHING=============
//===================================
bool UServerSubsystem::IsSearchingForServers()
{
	return bSearchingForServers;
}

bool UServerSubsystem::IsLANDiscoveryActive() const
{
	return bLANDiscoveryActive;
}

EServerSearchState UServerSubsystem::GetSearchState() const
{
	return SearchState;
}

FServerOperationResult UServerSubsystem::GetLastSearchResult() const
{
	return LastSearchResult;
}

int32 UServerSubsystem::GetNumOfServersFound()
{
	return SearchResults.Num();
}

const TArray<UServerBrowserSearchResult*>& UServerSubsystem::GetSearchResults() const
{
	return SearchResults;
}

void UServerSubsystem::FindSessions(const FServerSearchSettings& Settings)
{
	const FGuid RequestId = FGuid::NewGuid();
	IOnlineSessionPtr Interface;
	ISocketSubsystem* Sockets;
	TSharedPtr<FInternetAddr> BindAddress;
	bool bAccepted;
	bool bModeChanged;

	if (bDeinitializing)
		return;
	if (Settings.ConnectionType == EServerConnectionType::Direct)
	{
		StopLANDiscovery();
		ReportFailure(RequestId, EServerOperation::Search, EServerError::Unsupported);

		return;
	}
	if (Settings.MaxResults < 1 || !FMath::IsFinite(Settings.TimeoutSeconds) || Settings.TimeoutSeconds < 1.0f)
	{
		ReportFailure(RequestId, EServerOperation::Search, EServerError::InvalidRequest);

		return;
	}
	if (!CanSearchSessions())
	{
		ReportFailure(RequestId, EServerOperation::Search, EServerError::OperationInProgress);

		return;
	}

	if (Settings.ConnectionType == EServerConnectionType::Online)
		bLANDiscoveryActive = false;
	RequestedSearchSettings = Settings;
	if (bSearchingForServers || bPublishingSearchResult)
	{
		// Keep the active object and settings immutable until the provider releases them.
		bRefreshPending = true;

		return;
	}

	bModeChanged = Settings.ConnectionType != ActiveSearchSettings.ConnectionType;
	if (bModeChanged)
	{
		// Publish after installing search ownership so UI refresh requests coalesce.
		ServerToJoin = nullptr;
		SearchResults.Empty();
	}
	if (!CanSearchSessions() || bSearchingForServers)
		return;
	SearchProvider = Settings.ConnectionType == EServerConnectionType::LAN ? FName(TEXT("NULL")) : GetDefault<UServerSettings>()->OnlineProvider;
	Interface = ResolveSessionInterface(Settings.ConnectionType, SearchProvider);
	ActiveSearchId = RequestId;
	ActiveSearchSettings = Settings;
	SearchStartedAt = FPlatformTime::Seconds();
	bRefreshPending = false;
	PendingSearchError = EServerError::None;
	SearchInterface = Interface;
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->bIsLanQuery = Settings.ConnectionType == EServerConnectionType::LAN;
	if (!SessionSearch->bIsLanQuery)
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->MaxSearchResults = Settings.MaxResults;
	SessionSearch->PingBucketSize = 5;
	// Install identity before publishing state: UI callbacks may stop discovery immediately.
	SetSearchState(EServerSearchState::Searching);
	if (bModeChanged)
		OnSearchResultsUpdated.Broadcast();
	if (ActiveSearchId != RequestId)
		return;
	if (!Interface.IsValid())
	{
		CompleteSearch(RequestId, EServerError::ProviderUnavailable);

		return;
	}
	if (Settings.ConnectionType == EServerConnectionType::Online && !IsOnlineUserReady(SearchProvider))
	{
		CompleteSearch(RequestId, EServerError::NotLoggedIn);

		return;
	}
	if (Interface->GetNumSessions() > 0)
	{
		CompleteSearch(RequestId, EServerError::OperationInProgress);

		return;
	}

	FindDelegateHandle = Interface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UServerSubsystem::OnFindSessionsComplete, RequestId));
	Sockets = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (Sockets)
		BindAddress = Sockets->GetLocalBindAddr(*GLog);
	UE_LOG(LogServerSubsystem, Log, TEXT("Session search %s started: provider=%s, world=%s, bind=%s, driver=%s, max=%d, watchdog=%.1fs"), *RequestId.ToString(), *SearchProvider.ToString(), *GetNameSafe(GetWorld()), BindAddress.IsValid() ? *BindAddress->ToString(false) : TEXT("unavailable"), *GetNameSafe(GetWorld()->GetNetDriver()), Settings.MaxResults, Settings.TimeoutSeconds);
	bAccepted = Interface->FindSessions(0, SessionSearch.ToSharedRef());
	if (ActiveSearchId != RequestId)
		return;
	if (!bAccepted)
		CompleteSearch(RequestId, EServerError::SearchFailed);
	else if (SessionSearch->SearchState != EOnlineAsyncTaskState::InProgress)
		CompleteSearch(RequestId, EServerError::OperationInProgress);
}

void UServerSubsystem::RefreshSessions()
{
	FindSessions(RequestedSearchSettings);
}

void UServerSubsystem::CancelSessionSearch()
{
	StopLANDiscovery();
}

void UServerSubsystem::StartLANDiscovery()
{
	FServerSearchSettings Settings = GetDefault<UServerSettings>()->LANSearch;

	if (bDeinitializing)
		return;
	if (!CanSearchSessions())
	{
		ReportFailure(FGuid::NewGuid(), EServerOperation::Search, EServerError::OperationInProgress);

		return;
	}
	Settings.ConnectionType = EServerConnectionType::LAN;
	if (Settings.MaxResults < 1 || !FMath::IsFinite(Settings.TimeoutSeconds) || Settings.TimeoutSeconds < 1.0f)
	{
		ReportFailure(FGuid::NewGuid(), EServerOperation::Search, EServerError::InvalidRequest);

		return;
	}
	bLANDiscoveryActive = true;
	ConsecutiveSearchFailures = 0;
	FindSessions(Settings);
}

void UServerSubsystem::StopLANDiscovery()
{
	bLANDiscoveryActive = false;
	bRefreshPending = false;
	NextSearchAt = 0.0;
	CancelActiveSearch(EServerError::Cancelled);
}

void UServerSubsystem::FindLANSessions()
{
	FServerSearchSettings Settings = GetDefault<UServerSettings>()->LANSearch;

	Settings.ConnectionType = EServerConnectionType::LAN;
	FindSessions(Settings);
}

//=====================JOINING=====================
void UServerSubsystem::JoinSession(const FServerJoinRequest& Request)
{
	const FGuid RequestId = FGuid::NewGuid();
	APlayerController* Player = GetGameInstance()->GetFirstLocalPlayerController();
	FName Provider = NAME_None;

	if (bDeinitializing)
		return;
	if (!Player || (Request.ConnectionType != EServerConnectionType::Direct && (!IsValid(Request.Server) || !SearchResults.Contains(Request.Server) || !Request.Server->SearchResult.IsValid() || Request.Server->ConnectionType != Request.ConnectionType)))
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::InvalidRequest);

		return;
	}
	// Accept an endpoint only; caller-supplied travel options must not override transport.
	if (Request.ConnectionType == EServerConnectionType::Direct && (Request.Address.IsEmpty() || Request.Address.Contains(TEXT("?")) || Request.Address.Contains(TEXT("/")) || Request.Address.Contains(TEXT("\\")) || Request.Address.Contains(TEXT(" ")) || Request.Address.Contains(TEXT("\t")) || Request.Address.Contains(TEXT("\n")) || Request.Address.Contains(TEXT("\r"))))
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::InvalidRequest);

		return;
	}
	if (ServerState == EServerState::Hosting || ServerState == EServerState::Connected)
	{
		QueuedJoinRequest = Request;
		bJoinQueued = true;
		bHostQueued = false;
		BeginLeave(EServerOperation::Leave);

		return;
	}
	if (ServerState != EServerState::Idle || !GetWorld() || GetWorld()->GetNetMode() != NM_Standalone)
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::OperationInProgress);

		return;
	}
	if (Request.ConnectionType != EServerConnectionType::Direct)
		Provider = Request.Server->ProviderName;
	if (Request.ConnectionType == EServerConnectionType::Online && !IsOnlineUserReady(Provider))
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::NotLoggedIn);

		return;
	}
	StopLANDiscovery();
	if (bSearchingForServers || ServerState != EServerState::Idle)
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::OperationInProgress);

		return;
	}
	SessionInterface = ResolveSessionInterface(Request.ConnectionType, Provider);
	if (Request.ConnectionType != EServerConnectionType::Direct && !SessionInterface.IsValid())
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::ProviderUnavailable);

		return;
	}
	if (SessionInterface.IsValid() && SessionInterface->GetNamedSession(NAME_GameSession))
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::OperationInProgress);

		return;
	}
	if (!AcquireTransport(Request.ConnectionType))
	{
		ReportFailure(RequestId, EServerOperation::Join, EServerError::Unsupported);

		return;
	}
	BeginSessionOperation(RequestId, EServerOperation::Join, Request.ConnectionType);
	ActiveProvider = Provider;
	if (Request.ConnectionType == EServerConnectionType::Direct)
	{
		BeginSessionTravel(Request.Address);

		return;
	}
	JoinDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UServerSubsystem::OnJoinSessionComplete, RequestId));
	SetServerState(EServerState::Joining);
	if (!SessionInterface->JoinSession(0, NAME_GameSession, Request.Server->SearchResult) && ServerState == EServerState::Joining)
		OnJoinSessionComplete(NAME_GameSession, EOnJoinSessionCompleteResult::UnknownError, RequestId);
}

void UServerSubsystem::JoinLANSession()
{
	FServerJoinRequest Request;

	Request.ConnectionType = ServerToJoin ? ServerToJoin->ConnectionType : EServerConnectionType::Direct;
	Request.Server = ServerToJoin;
	Request.Address = DirectConnectURL;
	JoinSession(Request);
}

void UServerSubsystem::JoinDirectConnect(FString ServerURL)
{
	FServerJoinRequest Request;

	Request.ConnectionType = EServerConnectionType::Direct;
	Request.Address = MoveTemp(ServerURL);
	JoinSession(Request);
}

bool UServerSubsystem::HasSelectedServerToJoin()
{
	return IsValid(ServerToJoin) && SearchResults.Contains(ServerToJoin) && ServerToJoin->SearchResult.IsValid();
}

UServerBrowserSearchResult* UServerSubsystem::GetSelectedServer()
{
	return ServerToJoin;
}

void UServerSubsystem::SetServerSelection(UServerBrowserSearchResult* SearchResult)
{
	if (IsValid(SearchResult) && SearchResults.Contains(SearchResult))
		ServerToJoin = SearchResult;
}

void UServerSubsystem::ClearServerSelection()
{
	ServerToJoin = nullptr;
}

void UServerSubsystem::ClearServerBrowserResults()
{
	// Clearing the cache is not cancellation and must not release the active search.
	ServerToJoin = nullptr;
	SearchResults.Empty();
	OnSearchResultsUpdated.Broadcast();
}

void UServerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, FGuid RequestId)
{
	FString Address;
	EServerError Error = EServerError::ConnectionFailed;

	if (bDeinitializing || RequestId != SessionRequestId || SessionName != NAME_GameSession || !JoinDelegateHandle.IsValid())
		return;
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinDelegateHandle);
	JoinDelegateHandle.Reset();
	if (bSessionAbandoned)
	{
		DestroyActiveSession();

		return;
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (Result == EOnJoinSessionCompleteResult::SessionIsFull)
			Error = EServerError::ServerFull;
		else if (Result == EOnJoinSessionCompleteResult::SessionDoesNotExist)
			Error = EServerError::SessionNotFound;
		FailSessionOperation(Error);

		return;
	}
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address) || Address.IsEmpty())
	{
		FailSessionOperation(EServerError::ConnectionFailed);

		return;
	}
	ClearServerSelection();
	BeginSessionTravel(Address);
}

//=====================DISCOVERY CALLBACKS=====================
void UServerSubsystem::OnFindSessionsComplete(bool bWasSuccessful, FGuid RequestId)
{
	if (bDeinitializing || RequestId != ActiveSearchId || !bSearchingForServers)
		return;
	CompleteSearch(RequestId, PendingSearchError != EServerError::None ? PendingSearchError : (bWasSuccessful ? EServerError::None : EServerError::SearchFailed));
}

void UServerSubsystem::OnCancelFindSessionsComplete(bool bWasSuccessful, FGuid RequestId)
{
	if (bDeinitializing || RequestId != ActiveSearchId || SearchState != EServerSearchState::Cancelling)
		return;
	if (bWasSuccessful)
		CompleteSearch(RequestId, PendingSearchError);
	// If cancellation fails, retain ownership until the original search completes.
}

void UServerSubsystem::CancelActiveSearch(EServerError Reason)
{
	const FGuid RequestId = ActiveSearchId;

	if (!bSearchingForServers || SearchState == EServerSearchState::Cancelling)
		return;
	PendingSearchError = Reason;
	if (!SearchInterface.IsValid() || !SessionSearch.IsValid() || SessionSearch->SearchState != EOnlineAsyncTaskState::InProgress)
	{
		CompleteSearch(RequestId, Reason);

		return;
	}
	CancelFindDelegateHandle = SearchInterface->AddOnCancelFindSessionsCompleteDelegate_Handle(FOnCancelFindSessionsCompleteDelegate::CreateUObject(this, &UServerSubsystem::OnCancelFindSessionsComplete, RequestId));
	SetSearchState(EServerSearchState::Cancelling);
	if (RequestId != ActiveSearchId)
		return;
	if (!SearchInterface->CancelFindSessions() && RequestId == ActiveSearchId)
		UE_LOG(LogServerSubsystem, Warning, TEXT("Session search %s cancellation rejected; retaining ownership until completion."), *RequestId.ToString());
}

void UServerSubsystem::CompleteSearch(FGuid RequestId, EServerError Error)
{
	const double Now = FPlatformTime::Seconds();
	const UServerSettings* Settings = GetDefault<UServerSettings>();
	const FServerOperationResult Result = MakeOperationResult(RequestId, EServerOperation::Search, Error);
	const int32 Received = SessionSearch.IsValid() ? SessionSearch->SearchResults.Num() : 0;
	float RetryDelay;

	if (bDeinitializing || RequestId != ActiveSearchId || !bSearchingForServers)
		return;
	AgeSearchResults(Now);
	if (Error == EServerError::None)
		MergeSearchResults(Now);
	ConsecutiveSearchFailures = Error == EServerError::None ? 0 : FMath::Min(ConsecutiveSearchFailures + 1, 8);
	RetryDelay = FMath::Max(0.1f, Settings->RefreshInterval) * FMath::Pow(2.0f, static_cast<float>(ConsecutiveSearchFailures));
	NextSearchAt = Now + FMath::Min(RetryDelay, FMath::Max(1.0f, Settings->MaximumRetryInterval));
	UE_LOG(LogServerSubsystem, Log, TEXT("Session search %s finished: status=%d, received=%d, cached=%d, elapsed=%.2fs"), *RequestId.ToString(), static_cast<int32>(Error), Received, SearchResults.Num(), Now - SearchStartedAt);

	// Release first. UI events can stop discovery or request a coalesced refresh safely.
	bPublishingSearchResult = true;
	ReleaseSearch();
	LastSearchResult = Result;
	OnSearchStateChanged.Broadcast(SearchState);
	OnSearchResultsUpdated.Broadcast();
	OnSessionSearchCompleted.Broadcast(Result);
	if (Error != EServerError::None && Error != EServerError::Cancelled)
		OnServerOperationFailed.Broadcast(Result);
	bPublishingSearchResult = false;
}

void UServerSubsystem::ReleaseSearch()
{
	if (SearchInterface.IsValid())
	{
		SearchInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindDelegateHandle);
		SearchInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(CancelFindDelegateHandle);
	}
	FindDelegateHandle.Reset();
	CancelFindDelegateHandle.Reset();
	SearchInterface.Reset();
	SessionSearch.Reset();
	ActiveSearchId.Invalidate();
	PendingSearchError = EServerError::None;
	bSearchingForServers = false;
	SearchState = EServerSearchState::Idle;
}

void UServerSubsystem::MergeSearchResults(double Now)
{
	if (!SessionSearch.IsValid())
		return;
	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		const FString Id = Result.GetSessionIdStr();
		UServerBrowserSearchResult* Entry = nullptr;

		if (!Result.IsValid() || Id.IsEmpty())
			continue;
		for (UServerBrowserSearchResult* Existing : SearchResults)
		{
			if (Existing && Existing->ServerId == Id && Existing->ProviderName == SearchProvider && Existing->ConnectionType == ActiveSearchSettings.ConnectionType)
			{
				Entry = Existing;
				break;
			}
		}
		if (!Entry)
		{
			if (SearchResults.Num() >= ActiveSearchSettings.MaxResults)
				continue;
			Entry = NewObject<UServerBrowserSearchResult>(this);
			SearchResults.Add(Entry);
		}
		Entry->InitOnlineResult(Result);
		Entry->ProviderName = SearchProvider;
		Entry->ConnectionType = ActiveSearchSettings.ConnectionType;
		Entry->LastSeenAt = Now;
	}
}

bool UServerSubsystem::AgeSearchResults(double Now)
{
	const UServerSettings* Settings = GetDefault<UServerSettings>();
	const float StaleAfter = FMath::Max(0.1f, Settings->StaleAfterSeconds);
	const float ExpireAfter = FMath::Max(StaleAfter, Settings->ExpireAfterSeconds);
	bool bChanged = false;

	for (int32 Index = SearchResults.Num() - 1; Index >= 0; --Index)
	{
		UServerBrowserSearchResult* Entry = SearchResults[Index];
		const float Age = Entry ? static_cast<float>(FMath::Max(0.0, Now - Entry->LastSeenAt)) : ExpireAfter;

		if (!Entry || Age >= ExpireAfter)
		{
			if (ServerToJoin == Entry)
				ServerToJoin = nullptr;
			SearchResults.RemoveAt(Index);
			bChanged = true;
			continue;
		}
		Entry->SecondsSinceLastSeen = Age;
		if (Entry->bIsStale != (Age >= StaleAfter))
		{
			Entry->bIsStale = Age >= StaleAfter;
			bChanged = true;
		}
	}
	return bChanged;
}

bool UServerSubsystem::TickDiscovery(float DeltaSeconds)
{
	const double Now = FPlatformTime::Seconds();
	const double Timeout = FMath::Max(5.0f, GetDefault<UServerSettings>()->OperationTimeoutSeconds);
	UWorld* World = GetWorld();
	UNetDriver* Driver = World ? World->GetNetDriver() : nullptr;
	APlayerController* Player = GetGameInstance()->GetFirstLocalPlayerController();

	if (bDeinitializing)
		return false;
	if (ServerState == EServerState::TravelPending && World && World != TravelSourceWorld.Get() && World->HasBegunPlay())
	{
		// A pending connection is not success: wait for the destination world and
		// an open server connection with a local controller before notifying the UI.
		if (ActiveOperation == EServerOperation::Host && Driver && World->GetNetMode() == NM_ListenServer)
		{
			ReleaseTransport();
			SetServerState(EServerState::Hosting);
			FinishSessionOperation(EServerError::None);
		}
		else if (ActiveOperation == EServerOperation::Join && Driver && Driver->ServerConnection && Driver->ServerConnection->GetConnectionState() == USOCK_Open && Player && World->GetNetMode() == NM_Client)
		{
			ReleaseTransport();
			SetServerState(EServerState::Connected);
			FinishSessionOperation(EServerError::None);
		}
	}
	if (bReturningToMenu && World && World != TravelSourceWorld.Get() && World->HasBegunPlay() && World->GetNetMode() == NM_Standalone && !Driver)
	{
		bReturningToMenu = false;
		bSessionAbandoned = false;
		ActiveProvider = NAME_None;
		ActiveConnectionType = EServerConnectionType::Direct;
		SetServerState(EServerState::Idle);
		FinishSessionOperation(EServerError::None);
	}
	if (Now - OperationStartedAt >= Timeout)
	{
		if (ServerState == EServerState::Creating || ServerState == EServerState::Joining || ServerState == EServerState::TravelPending)
			FailSessionOperation(EServerError::TimedOut);
		else if (ServerState == EServerState::Leaving)
		{
			// Retain a pending destroy delegate: a backend timeout is not proof of
			// deletion. A late completion can still finish recovery safely.
			ReleaseTransport();
			bReturningToMenu = false;
			bHostQueued = false;
			bJoinQueued = false;
			SetServerState(EServerState::RecoveryFailed);
			if (bOperationReported)
				ReportFailure(SessionRequestId, ActiveOperation, EServerError::TimedOut);
			FinishSessionOperation(EServerError::TimedOut);
		}
	}
	if (ServerState == EServerState::Idle && !bSearchingForServers)
	{
		if (bHostQueued)
		{
			const FServerHostSettings Settings = QueuedHostSettings;

			bHostQueued = false;
			StartServer(Settings);
		}
		else if (bJoinQueued)
		{
			const FServerJoinRequest Request = QueuedJoinRequest;

			bJoinQueued = false;
			JoinSession(Request);
			QueuedJoinRequest.Server = nullptr;
		}
	}
	if ((bLANDiscoveryActive || bSearchingForServers || bRefreshPending) && !CanSearchSessions())
		StopLANDiscovery();
	if (bSearchingForServers && SearchState == EServerSearchState::Searching && Now - SearchStartedAt >= ActiveSearchSettings.TimeoutSeconds)
		CancelActiveSearch(EServerError::TimedOut);
	if (AgeSearchResults(Now))
		OnSearchResultsUpdated.Broadcast();
	if (!bSearchingForServers && !bPublishingSearchResult && (bRefreshPending || (bLANDiscoveryActive && Now >= NextSearchAt)))
		RefreshSessions();
	if (bServerStateDirty)
	{
		bServerStateDirty = false;
		OnServerStateChanged.Broadcast(ServerState);
	}

	return true;
}

void UServerSubsystem::OnWorldTearDown(UWorld* World)
{
	if (World && World->GetGameInstance() == GetGameInstance())
		StopLANDiscovery();
}

//===========================
//=========UTILITIES=========
//===========================
bool UServerSubsystem::IsServer()
{
	return GetWorld() && GetWorld()->GetNetMode() < NM_Client;
}

AFirstPersonGame* UServerSubsystem::GetGameMode()
{
	return GetWorld() ? GetWorld()->GetAuthGameMode<AFirstPersonGame>() : nullptr;
}

AFirstPersonGameState* UServerSubsystem::GetGameState()
{
	return GetWorld() ? GetWorld()->GetGameState<AFirstPersonGameState>() : nullptr;
}

IOnlineSessionPtr UServerSubsystem::ResolveLANInterface() const
{
	return GetWorld() ? Online::GetSessionInterface(GetWorld(), FName(TEXT("NULL"))) : nullptr;
}

bool UServerSubsystem::CanSearchSessions() const
{
	const UWorld* World = GetWorld();

	return !bDeinitializing && ServerState == EServerState::Idle && World && !World->bIsTearingDown && World->GetNetMode() == NM_Standalone;
}

IOnlineSessionPtr UServerSubsystem::ResolveSessionInterface(EServerConnectionType Type, FName Provider) const
{
	if (Type == EServerConnectionType::Direct || !GetWorld())
		return nullptr;
	if (Type == EServerConnectionType::LAN)
		return ResolveLANInterface();
	if (Provider.IsNone())
		Provider = GetDefault<UServerSettings>()->OnlineProvider;
	// Other providers require their transport and identity flow to be integrated first.
	if (Provider != FName(TEXT("STEAM")))
		return nullptr;

	return Online::GetSessionInterface(GetWorld(), Provider);
}

bool UServerSubsystem::IsOnlineUserReady(FName Provider) const
{
	IOnlineSubsystem* Subsystem = GetWorld() && Provider == FName(TEXT("STEAM")) ? Online::GetSubsystem(GetWorld(), Provider) : nullptr;
	IOnlineIdentityPtr Identity = Subsystem ? Subsystem->GetIdentityInterface() : nullptr;

	return Identity.IsValid() && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn && Identity->GetUniquePlayerId(0).IsValid();
}

bool UServerSubsystem::AcquireTransport(EServerConnectionType Type)
{
	const FName DriverClass = Type == EServerConnectionType::Online ? FName(TEXT("/Script/SteamSockets.SteamSocketsNetDriver")) : FName(TEXT("/Script/OnlineSubsystemUtils.IpNetDriver"));
	UClass* Class = nullptr;
	FNetDriverDefinition* Definition = nullptr;
	FString Overrides;

	if (!GEngine || (TransportOwner.IsValid() && TransportOwner.Get() != this))
		return false;
	// Command-line overrides bypass our definition, so refuse rather than silently
	// attempting a Steam address with an IP driver (or the reverse).
	if (FParse::Value(FCommandLine::Get(), TEXT("NetDriverOverrides="), Overrides))
		return false;
	Class = StaticLoadClass(UNetDriver::StaticClass(), nullptr, *DriverClass.ToString());
	if (!Class || !Class->GetDefaultObject<UNetDriver>()->IsAvailable())
		return false;
	// SteamSockets can replace the process default socket subsystem at startup.
	// bUseSteamNetworking=false keeps plain UDP available alongside SteamSockets.
	if (Type != EServerConnectionType::Online && ISocketSubsystem::Get() != ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))
	{
		UE_LOG(LogServerSubsystem, Warning, TEXT("LAN/direct requires OnlineSubsystemSteam.bUseSteamNetworking=false so IP sockets remain platform sockets."));

		return false;
	}
	if (TransportOwner.Get() == this)
		return true;
	for (FNetDriverDefinition& Candidate : GEngine->NetDriverDefinitions)
	{
		if (Candidate.DefName == NAME_GameNetDriver)
		{
			Definition = &Candidate;
			break;
		}
	}
	bHadGameDriver = Definition != nullptr;
	if (Definition)
		SavedGameDriver = *Definition;
	else
	{
		Definition = &GEngine->NetDriverDefinitions.AddDefaulted_GetRef();
		Definition->DefName = NAME_GameNetDriver;
	}
	TransportOwner = this;
	Definition->DriverClassName = DriverClass;
	// Fail explicitly if this transport is unavailable; IP is not a Steam fallback.
	Definition->DriverClassNameFallback = DriverClass;

	return true;
}

void UServerSubsystem::ReleaseTransport()
{
	if (!GEngine || TransportOwner.Get() != this)
		return;
	for (int32 Index = 0; Index < GEngine->NetDriverDefinitions.Num(); ++Index)
	{
		if (GEngine->NetDriverDefinitions[Index].DefName == NAME_GameNetDriver)
		{
			if (bHadGameDriver)
				GEngine->NetDriverDefinitions[Index] = SavedGameDriver;
			else
				GEngine->NetDriverDefinitions.RemoveAt(Index);
			break;
		}
	}
	TransportOwner.Reset();
}

void UServerSubsystem::BeginSessionOperation(FGuid RequestId, EServerOperation Operation, EServerConnectionType Type)
{
	SessionRequestId = RequestId;
	PendingLeaveRequestId.Invalidate();
	ActiveOperation = Operation;
	ActiveConnectionType = Type;
	// Resolving an interface alone is not ownership (another PIE instance may own its session).
	bOwnsSession = Type != EServerConnectionType::Direct;
	OperationStartedAt = FPlatformTime::Seconds();
	bOperationReported = false;
	bSessionAbandoned = false;
	bReturningToMenu = false;
	bHostQueued = false;
	bJoinQueued = false;
}

void UServerSubsystem::BeginSessionTravel(const FString& URL)
{
	UWorld* World = GetWorld();
	APlayerController* Player = GetGameInstance()->GetFirstLocalPlayerController();

	TravelSourceWorld = World;
	OperationStartedAt = FPlatformTime::Seconds();
	SetServerState(EServerState::TravelPending);
	if (!World || (ActiveOperation == EServerOperation::Join && !Player))
	{
		FailSessionOperation(EServerError::ConnectionFailed);

		return;
	}
	if (ActiveOperation == EServerOperation::Host)
	{
		// A fresh listen load creates the selected driver from the standalone menu.
		// Seamless ServerTravel can otherwise preserve the menu's null driver.
		UGameplayStatics::OpenLevel(World, FName(*URL), true, TEXT("listen"));
	}
	else
		Player->ClientTravel(URL, TRAVEL_Absolute);
}

void UServerSubsystem::ClearSessionDelegates()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateDelegateHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinDelegateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroyDelegateHandle);
	}
	CreateDelegateHandle.Reset();
	JoinDelegateHandle.Reset();
	DestroyDelegateHandle.Reset();
}

void UServerSubsystem::SetServerState(EServerState State)
{
	if (ServerState == State)
		return;
	ServerState = State;
	// Publish from the end of TickDiscovery. UI listeners can then start/leave
	// without interrupting a provider setup halfway through its mutation.
	bServerStateDirty = true;
}

void UServerSubsystem::SetSearchState(EServerSearchState State)
{
	SearchState = State;
	bSearchingForServers = State != EServerSearchState::Idle;
	OnSearchStateChanged.Broadcast(State);
}

FServerOperationResult UServerSubsystem::MakeOperationResult(FGuid RequestId, EServerOperation Operation, EServerError Error) const
{
	FServerOperationResult Result;

	Result.RequestId = RequestId;
	Result.Operation = Operation;
	Result.Error = Error;
	Result.bSucceeded = Error == EServerError::None;
	switch (Error)
	{
	case EServerError::None:
		break;
	case EServerError::InvalidRequest:
		Result.Message = NSLOCTEXT("ServerSubsystem", "InvalidRequest", "The server request or settings are invalid.");
		break;
	case EServerError::Unsupported:
		Result.Message = NSLOCTEXT("ServerSubsystem", "Unsupported", "This connection mode or network driver is not supported by the current implementation.");
		break;
	case EServerError::ProviderUnavailable:
		Result.Message = NSLOCTEXT("ServerSubsystem", "ProviderUnavailable", "The requested session provider is unavailable.");
		break;
	case EServerError::OperationInProgress:
		Result.Message = NSLOCTEXT("ServerSubsystem", "Busy", "Another session operation or active connection prevents this request.");
		break;
	case EServerError::SearchFailed:
		Result.Message = NSLOCTEXT("ServerSubsystem", "SearchFailed", "Session discovery failed. Check the connection and try again.");
		break;
	case EServerError::TimedOut:
		Result.Message = NSLOCTEXT("ServerSubsystem", "TimedOut", "The session operation timed out.");
		break;
	case EServerError::Cancelled:
		Result.Message = NSLOCTEXT("ServerSubsystem", "Cancelled", "The session operation was cancelled.");
		break;
	case EServerError::NotLoggedIn:
		Result.Message = NSLOCTEXT("ServerSubsystem", "NotLoggedIn", "Sign in to the online provider before using online sessions.");
		break;
	case EServerError::ServerFull:
		Result.Message = NSLOCTEXT("ServerSubsystem", "ServerFull", "Failed to join: the server is full.");
		break;
	case EServerError::SessionNotFound:
		Result.Message = NSLOCTEXT("ServerSubsystem", "SessionNotFound", "The session is no longer available.");
		break;
	case EServerError::CleanupFailed:
		Result.Message = NSLOCTEXT("ServerSubsystem", "CleanupFailed", "The previous session could not be cleaned up. Retry leaving before starting another session.");
		break;
	default:
		Result.Message = NSLOCTEXT("ServerSubsystem", "ConnectionFailed", "The session connection failed.");
		break;
	}
	return Result;
}

void UServerSubsystem::ReportFailure(FGuid RequestId, EServerOperation Operation, EServerError Error)
{
	const FServerOperationResult Result = MakeOperationResult(RequestId, Operation, Error);

	UE_LOG(LogServerSubsystem, Warning, TEXT("Server request %s: operation=%d, error=%d"), *RequestId.ToString(), static_cast<int32>(Operation), static_cast<int32>(Error));
	OnServerOperationFailed.Broadcast(Result);
}
