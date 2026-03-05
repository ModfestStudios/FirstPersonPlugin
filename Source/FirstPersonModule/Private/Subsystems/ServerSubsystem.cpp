// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ServerSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

#include "Online/ServerBrowserSearchResult.h"

/*asset handling*/
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

/*engine*/
#include "Engine/World.h"


/*gamemodes*/
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"

/*maps*/
#include "Maps/MapAsset.h"

/*players*/
#include "Players/FirstPersonPlayerController.h"



/*utilities*/
#include "../Logging.h"
#include "Kismet/GameplayStatics.h"


/*define sessiont consts*/
const FName UServerSubsystem::SERVER_NAME(TEXT("SERVER_NAME"));
const FName UServerSubsystem::GAME_MODE(TEXT("GAME_MODE"));
const FName UServerSubsystem::GAME_VERSION(TEXT("GAME_VERSION"));

void UServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	/*GET ONLINE SUBSYSTEM*/
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();

		/*bind event delegates*/
		if (SessionInterface)
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerSubsystem::OnJoinSessionComplete);
		}
	}
}

void UServerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UServerSubsystem::SetCurrentMap(UMapAsset* MapAsset)
{
	/*safety check*/
	if (!MapAsset)
		return;

	if (IsServer())
	{
		PreviousMap = CurrentMap;
		CurrentMap = MapAsset;

		/*notify gamemode which will update gamestate which will in turn update client's ServerSubsystem calling this same function back*/
		if (GetGameMode())
			GetGameMode()->OnCurrentMapUpdated(MapAsset);
	}
}

const UMapAsset* UServerSubsystem::GetCurrentMap() const
{
	return CurrentMap;	
}

/*Client_CurrentMapUpdated() - Pseudo-Client function that is updated by the GameState notifying it when the server has updated the "Current Map" property*/
void UServerSubsystem::Client_OnCurrentMapUpdated(UMapAsset* MapAsset)
{
	PreviousMap = CurrentMap;
	CurrentMap = MapAsset;
}

const TArray<UMapAsset*> UServerSubsystem::GetMapRotation() const
{	
	return MapRotation;
}

void UServerSubsystem::SetMapRotation(TArray<UMapAsset*> MapList)
{
	if (IsServer())
	{
		MapRotation.Empty(); // clear old list
		MapRotation = MapList;

		/*notify gamemode which will update gamestate which will in turn update client's ServerSubsystem calling this same function back*/
		if (GetGameMode())
			GetGameMode()->OnMapRotationUpdated(MapList);
	}	
}

void UServerSubsystem::AddToMapRotation(UMapAsset* MapAsset)
{
	if (!MapAsset)
		return;

	if (IsServer())
	{		
		MapRotation.Add(MapAsset);

		/*notify gamemode/gamestate*/
		if (AFirstPersonGame* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AFirstPersonGame>() : nullptr)
			GM->OnMapRotationUpdated(MapRotation);
	}	
}

void UServerSubsystem::Client_OnMapRotationUpdated(TArray<UMapAsset*> MapList)
{
	MapRotation.Empty(); // clear old list
	MapRotation = MapList;
}

const UMapAsset* UServerSubsystem::GetNextMap() const
{
	return NextMap;
}

void UServerSubsystem::SetNextMap(UMapAsset* MapAsset)
{	
	if (!MapAsset)
		return;

	if (IsServer()) 
	{
		NextMap = MapAsset;
		if (GetGameMode())
			GetGameMode()->OnNextMapUpdated(MapAsset);
	}
	
	/*clients*/
	else
	{
		/*Request a new map via the PlayerController so the server can evaluate if this player has permissions to do so*/
		if (AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			PC->ServerSelectNextMap(MapAsset);
		}
	}	
}

void UServerSubsystem::ClearNextMap()
{
	if (IsServer())
	{
		NextMap = nullptr;
	}
}

void UServerSubsystem::Client_OnNextMapUpdated(UMapAsset* MapAsset)
{
	NextMap = MapAsset;
}

const UMapAsset* UServerSubsystem::GetHoveredMap() const
{
	return HoveredMap;
}

void UServerSubsystem::SetHoveredMap(UMapAsset* MapAsset)
{
	HoveredMap = MapAsset;
}

void UServerSubsystem::ClearHoveredMap()
{
	HoveredMap = nullptr;
}

bool UServerSubsystem::IsMapSelected(UMapAsset* MapInfo)
{
	if (CurrentMap == MapInfo)
		return true;
	else if (NextMap == MapInfo)
		return true;
	else if (MapRotation.Contains(MapInfo))
		return true;
	else
		return false;
}

FString UServerSubsystem::SantizeMapPath(FString Path)
{	
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::SantizeMapPath(%s)"), *Path);
	int32 DotIndex;
	
	if (Path.FindChar('.', DotIndex))
	{		
		Path.LeftInline(DotIndex);
		UE_LOG(LogServerSubsystem, Log, TEXT("Santized Path : %s"), *Path);
		return Path;
	}
	return Path;
}


FSessionInfo UServerSubsystem::GetCurrentSessionInfo()
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::GetCurrentSessionInfo()"));
	
	FSessionInfo SessionInfo;
	SessionInfo.SessionType = TEXT("No Active Session");
	SessionInfo.ServerName = TEXT("N/A");
	SessionInfo.MaxConnections = 0;
	SessionInfo.OpenConnections = 0;
	SessionInfo.bIsAdvertising = false;
	SessionInfo.ServerStatus = TEXT("NOT LISTENING");

	if (!IsSessionValid())
	{
		UE_LOG(LogServerSubsystem, Warning, TEXT("!!!Online Session Interface is invalid!!!"));
		return SessionInfo;
	}

	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (CurrentSession)
	{
		SessionInfo.SessionType = CurrentSession->SessionSettings.bIsLANMatch ? TEXT("LAN") : TEXT("Online");				
		SessionInfo.MaxConnections = CurrentSession->SessionSettings.NumPublicConnections;
		SessionInfo.OpenConnections = SessionInfo.MaxConnections - CurrentSession->NumOpenPublicConnections;
		SessionInfo.bIsAdvertising = CurrentSession->SessionSettings.bShouldAdvertise;

		/*metadata*/
		CurrentSession->SessionSettings.Get(SERVER_NAME, SessionInfo.ServerName); //Server Name

		if (const UWorld* World = GetWorld())
		{
			switch (World->GetNetMode())
			{
				case NM_ListenServer:
					SessionInfo.ServerStatus = TEXT("Listen Server");
					break;
				case NM_DedicatedServer:
					SessionInfo.ServerStatus = TEXT("Dedicated Server");
					break;
				default:
					SessionInfo.ServerStatus = TEXT("Singleplayer (Not Listening)");
					break;
			}
		}
	}
	else
	{
		UE_LOG(LogServerSubsystem, Warning, TEXT("No Active Session Found"));
	}

	UE_LOG(LogServerSubsystem, Log, TEXT("Session Info - Type: %s | Server: %s | Players: %d/%d | Advertising: %s | Status: %s"),
		*SessionInfo.SessionType,
		*SessionInfo.ServerName,
		SessionInfo.MaxConnections - SessionInfo.OpenConnections,
		SessionInfo.MaxConnections,
		SessionInfo.bIsAdvertising ? TEXT("Yes") : TEXT("No"),
		*SessionInfo.ServerStatus);

	return SessionInfo;
}

bool UServerSubsystem::IsSessionValid()
{
	return SessionInterface.IsValid();	
}

void UServerSubsystem::HostLANSession()
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::HostLanSession()"));

	/*safety check*/
	if (!SessionInterface.IsValid())
		return;


	/*quick setup of settings*/
	SessionSettings = FOnlineSessionSettings();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bUsesPresence = false;
	SessionSettings.NumPublicConnections = 32;
	SessionSettings.bShouldAdvertise = true;

	/*server metadata*/
	SessionSettings.Set(SERVER_NAME, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);

	UE_LOG(LogServerSubsystem, Log, TEXT("~UServerSubsystem::HostLanSession()"));
}

bool UServerSubsystem::IsSearchingForServers()
{
	return bSearchingForServers;
}

int32 UServerSubsystem::GetNumOfServersFound()
{
	return SessionSearch ? SessionSearch->SearchResults.Num() : 0;
}

void UServerSubsystem::FindLANSessions()
{
	/*safety check*/
	if (!IsSessionValid())
		return;

	/*setup search criteria*/
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 30;
	SessionSearch->PingBucketSize = 5;

	/*flag so UI knows we're searching*/
	bSearchingForServers = true;

	/*run search - will return via a delegate that calls OnFindSessionComplete*/
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UServerSubsystem::JoinLANSession()
{
	/*we have a server selected from a list*/
	if (ServerToJoin)
	{
		if (ServerToJoin->SearchResult.IsValid())
			JoinSession(ServerToJoin->SearchResult);
	}
	/*else assume Direct Connect IP*/
	else
	{
		JoinDirectConnect(DirectConnectURL);
	}
}

void UServerSubsystem::JoinSession(const FOnlineSessionSearchResult& Result)
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::JoinSession()"));
	if (!IsSessionValid())
	{
		UE_LOG(LogServerSubsystem, Error, TEXT("Session Interface is invalid! Cannot join server"));
		return;
	}

	if (!Result.IsValid())
	{
		UE_LOG(LogServerSubsystem, Error, TEXT("Invalid session result - cannot join server!"));
		return;
	}

	UE_LOG(LogServerSubsystem, Log, TEXT("Attempting to Join Session: %s..."), *Result.Session.OwningUserName);
	SessionInterface->JoinSession(0, NAME_GameSession, Result);

	UE_LOG(LogServerSubsystem, Log, TEXT("~UServerSubsystem::JoinSession()"));
}

void UServerSubsystem::JoinDirectConnect(FString ServerURL)
{
	FString ConsoleCommand = "open " + ServerURL;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); //get client's player controller
	if(PlayerController)
		PlayerController->ConsoleCommand(ConsoleCommand, true);
}

bool UServerSubsystem::HasSelectedServerToJoin()
{
	if (ServerToJoin != nullptr && ServerToJoin->SearchResult.IsValid())
		return true;
	else
		return false;
}

UServerBrowserSearchResult* UServerSubsystem::GetSelectedServer()
{
	return ServerToJoin;
}

void UServerSubsystem::OnServerSelected(UServerBrowserSearchResult* SearchResult)
{
	if (SearchResult)
	{
		ServerToJoin = SearchResult;
	}
}

void UServerSubsystem::ClearServerSelection()
{
	ServerToJoin = nullptr;
}

void UServerSubsystem::ClearServerBrowserResults()
{
	bSearchingForServers = false;
	SearchResults.Empty();
}

void UServerSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::OnCreateSessionComplete()"));	

	/*session was successfully created*/
	if (bWasSuccessful)
	{
		UE_LOG(LogServerSubsystem, Log, TEXT("[SUCCESS] - LAN Session %s Created Successfully"), *SessionName.ToString());


		if (UWorld* World = GetWorld())
		{
			FString MapURL = GetNextMap()->Level.ToString();
			UE_LOG(LogServerSubsystem, Log, TEXT("Preparing Level [%s] for travel..."),*MapURL);
			MapURL = SantizeMapPath(MapURL);			
			MapURL += "?listen"; //make sure we're listening for clients
			if (MapURL.IsEmpty())
			{
				UE_LOG(LogServerSubsystem, Error, TEXT("Level URL is empty"));
				return;
			}

			UE_LOG(LogServerSubsystem, Log, TEXT("initiaing Server Travel..."));
			/*perform travel*/
			World->ServerTravel(MapURL,false,false);
		}
	}

	/*session failed to be created*/
	else
	{
		UE_LOG(LogServerSubsystem, Error, TEXT("[Failed] - LAN Session %s Failed to be Created"), *SessionName.ToString());
	}

	UE_LOG(LogServerSubsystem, Log, TEXT("~UServerSubsystem::OnCreateSessionComplete()"))
}

void UServerSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::OnFindSessionsComplete()"));

	
	ClearServerBrowserResults();

	if (bWasSuccessful)
	{
		UE_LOG(LogServerSubsystem, Log, TEXT("[Success] Session Search successful - found %i sessions"),SessionSearch->SearchResults.Num());

		for (auto& Session : SessionSearch->SearchResults)
		{
			FString SessionName = Session.Session.OwningUserName.IsEmpty() ? TEXT("Unknown") : Session.Session.OwningUserName;
			UE_LOG(LogServerSubsystem, Log, TEXT("Found Session: %s | Ping: %d | Open Slots: %d/%d"),
				*SessionName,
				Session.PingInMs,
				Session.Session.NumOpenPublicConnections,
				Session.Session.SessionSettings.NumPublicConnections);

			UServerBrowserSearchResult* Server = NewObject<UServerBrowserSearchResult>();
			Server->InitOnlineResult(Session);


			SearchResults.Add(Server);
		}

		if (OnSearchResultsUpdated.IsBound())
			OnSearchResultsUpdated.Broadcast();
	}
	else
	{
		UE_LOG(LogServerSubsystem, Error, TEXT("[Failure] Session search did not return any results"));
	}
	
}

void UServerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogServerSubsystem, Log, TEXT("UServerSubsystem::OnJoinSessionComplete()"));

	ClearServerSelection();

	UE_LOG(LogServerSubsystem, Log, TEXT("Getting connection string...."));
	FString ConnectionInfo;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectionInfo))
	{
		UE_LOG(LogServerSubsystem, Log, TEXT("%s"), *ConnectionInfo);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); //get client's player controller
		if (PlayerController)
		{
			UE_LOG(LogServerSubsystem, Log, TEXT("client travel initiated..."));
			PlayerController->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogServerSubsystem, Error, TEXT("Failed to acquire Player Controller"));
		}
			
	}
	else
	{
		UE_LOG(LogServerSubsystem, Error, TEXT("Failed to join session: %s"), *SessionName.ToString());
	}


	UE_LOG(LogServerSubsystem, Log, TEXT("~UServerSubsystem::OnJoinSessionComplete()"));
}

AFirstPersonGame* UServerSubsystem::GetGameMode()
{
	return GetWorld() ? GetWorld()->GetAuthGameMode<AFirstPersonGame>() : nullptr; 
}

AFirstPersonGameState* UServerSubsystem::GetGameState()
{
	return GetWorld() ? GetWorld()->GetGameState<AFirstPersonGameState>() : nullptr;
}
