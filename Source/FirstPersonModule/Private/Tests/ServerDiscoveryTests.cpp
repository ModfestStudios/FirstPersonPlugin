// Copyright Modfest Studios
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Online/ServerBrowserSearchResult.h"
#include "Online/ServerSettings.h"
#include "Subsystems/ServerSubsystem.h"
#include "UObject/StrongObjectPtr.h"

namespace ServerDiscoveryTests
{
// In-memory session data exercises the cache without requiring another machine.
class FTestSessionInfo final : public FOnlineSessionInfo
{
	FUniqueNetIdRef Id = FUniqueNetIdString::Create(TEXT("ServerDiscoveryTest"), TEXT("NULL"));
public:
	virtual const uint8* GetBytes() const override { return Id->GetBytes(); }
	virtual int32 GetSize() const override { return Id->GetSize(); }
	virtual bool IsValid() const override { return true; }
	virtual FString ToString() const override { return Id->ToString(); }
	virtual FString ToDebugString() const override { return Id->ToString(); }
	virtual const FUniqueNetId& GetSessionId() const override { return *Id; }
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FServerDiscoveryAutomationTest, "FirstPersonModule.Server.DiscoveryOwnershipAndCache", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FServerDiscoveryAutomationTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UGameInstance> Instance(NewObject<UGameInstance>(GEngine));
	UServerSubsystem* Server;
	UWorld* World;
	FServerSearchSettings Settings;
	FOnlineSessionSearchResult Found;
	TSharedPtr<FOnlineSessionSearch> OriginalSearch;
	UServerBrowserSearchResult* OriginalEntry;
	FGuid OriginalId;
	FGuid NewId;
	const double Now = FPlatformTime::Seconds();
	const UServerSettings* Defaults = GetDefault<UServerSettings>();

	Instance->InitializeStandalone();
	World = Instance->GetWorld();
	Server = Instance->GetSubsystem<UServerSubsystem>();
	if (!TestNotNull(TEXT("Server subsystem exists"), Server))
	{
		Instance->Shutdown();
		World->DestroyWorld(false);
		GEngine->DestroyWorldContext(World);
		return false;
	}

	// Real Null search, cancellation and restart exercise synchronous delegate behavior.
	Server->FindSessions(Settings);
	OriginalSearch = Server->SessionSearch;
	OriginalId = Server->ActiveSearchId;
	TestTrue(TEXT("Null provider accepted search"), Server->bSearchingForServers);
	Settings.MaxResults = 64;
	Server->FindSessions(Settings);
	TestTrue(TEXT("Refresh keeps the provider-owned search"), Server->SessionSearch == OriginalSearch);
	TestEqual(TEXT("Active settings remain immutable"), Server->ActiveSearchSettings.MaxResults, 100);
	TestEqual(TEXT("Next refresh captures new settings"), Server->RequestedSearchSettings.MaxResults, 64);
	TestTrue(TEXT("Refresh requests coalesce"), Server->bRefreshPending);
	Server->CancelSessionSearch();
	TestFalse(TEXT("Cancellation releases Null search"), Server->bSearchingForServers);
	TestFalse(TEXT("Cancellation clears pending refresh"), Server->bRefreshPending);
	TestEqual(TEXT("Cancellation has a distinct result"), Server->LastSearchResult.Error, EServerError::Cancelled);
	Server->OnFindSessionsComplete(true, OriginalId);
	TestEqual(TEXT("Late callback cannot overwrite cancellation"), Server->LastSearchResult.Error, EServerError::Cancelled);

	Server->FindSessions(Settings);
	NewId = Server->ActiveSearchId;
	Server->OnFindSessionsComplete(true, OriginalId);
	TestTrue(TEXT("Old callback cannot finish a new search"), Server->ActiveSearchId == NewId && Server->bSearchingForServers);
	Server->CancelActiveSearch(EServerError::TimedOut);
	TestEqual(TEXT("Timeout is distinct from cancellation"), Server->LastSearchResult.Error, EServerError::TimedOut);
	TestTrue(TEXT("Failure backoff schedules in the future"), Server->NextSearchAt > FPlatformTime::Seconds());

	// Simulate completed provider data after the actual provider has been cancelled.
	Found.Session.SessionInfo = MakeShared<ServerDiscoveryTests::FTestSessionInfo>();
	Found.Session.OwningUserId = FUniqueNetIdString::Create(TEXT("Host"), TEXT("NULL"));
	Found.Session.SessionSettings.NumPublicConnections = 64;
	Found.Session.NumOpenPublicConnections = 60;
	Found.Session.SessionSettings.Set(UServerSubsystem::SERVER_NAME, FString(TEXT("Original name")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Server->SessionSearch = MakeShared<FOnlineSessionSearch>();
	Server->SessionSearch->SearchResults.Add(Found);
	Server->MergeSearchResults(Now);
	TestEqual(TEXT("Result is cached"), Server->GetSearchResults().Num(), 1);
	if (Server->GetSearchResults().Num() == 1)
	{
		OriginalEntry = Server->GetSearchResults()[0];
		TestEqual(TEXT("LAN result retains the Null provider"), OriginalEntry->ProviderName, FName(TEXT("NULL")));
		TestEqual(TEXT("LAN result retains its connection mode"), OriginalEntry->ConnectionType, EServerConnectionType::LAN);
		Server->SetServerSelection(OriginalEntry);
		Found.Session.SessionSettings.Set(UServerSubsystem::SERVER_NAME, FString(TEXT("Updated name")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		Server->SessionSearch->SearchResults[0] = Found;
		Server->MergeSearchResults(Now + 1.0);
		TestEqual(TEXT("Refresh does not duplicate entries"), Server->GetSearchResults().Num(), 1);
		TestTrue(TEXT("Refresh preserves object and selection"), Server->GetSearchResults()[0] == OriginalEntry && Server->GetSelectedServer() == OriginalEntry);
		TestEqual(TEXT("Metadata refreshes in place"), OriginalEntry->ServerName, FString(TEXT("Updated name")));
		TestEqual(TEXT("Player count remains accurate"), OriginalEntry->CurrentPlayers, 4);

		Server->ActiveSearchId = FGuid::NewGuid();
		Server->SearchStartedAt = Now;
		Server->SetSearchState(EServerSearchState::Searching);
		Server->CompleteSearch(Server->ActiveSearchId, EServerError::SearchFailed);
		TestEqual(TEXT("Failed search retains cached results"), Server->GetSearchResults().Num(), 1);
		Server->SessionSearch = MakeShared<FOnlineSessionSearch>();
		Server->ActiveSearchId = FGuid::NewGuid();
		Server->SetSearchState(EServerSearchState::Searching);
		Server->CompleteSearch(Server->ActiveSearchId, EServerError::None);
		TestTrue(TEXT("Successful empty response remains success"), Server->LastSearchResult.bSucceeded);
		TestEqual(TEXT("Empty response does not wipe the cache"), Server->GetSearchResults().Num(), 1);

		Server->AgeSearchResults(Now + 1.0 + Defaults->StaleAfterSeconds + 0.1);
		TestTrue(TEXT("Missing entry becomes stale"), OriginalEntry->bIsStale);
		Server->AgeSearchResults(Now + 1.0 + FMath::Max(Defaults->StaleAfterSeconds, Defaults->ExpireAfterSeconds) + 0.1);
		TestEqual(TEXT("Expired entry is removed"), Server->GetSearchResults().Num(), 0);
		TestNull(TEXT("Expired selection is cleared"), Server->GetSelectedServer());
	}

	// Identical provider session IDs must not alias across connection providers.
	Server->SessionSearch = MakeShared<FOnlineSessionSearch>();
	Server->SessionSearch->SearchResults.Add(Found);
	Server->SearchProvider = TEXT("NULL");
	Server->ActiveSearchSettings.ConnectionType = EServerConnectionType::LAN;
	Server->MergeSearchResults(FPlatformTime::Seconds());
	Server->SearchProvider = TEXT("STEAM");
	Server->ActiveSearchSettings.ConnectionType = EServerConnectionType::Online;
	Server->MergeSearchResults(FPlatformTime::Seconds());
	TestEqual(TEXT("Provider-scoped IDs cannot overwrite another provider's result"), Server->GetSearchResults().Num(), 2);
	Server->ClearServerBrowserResults();
	Server->ReleaseSearch();

	Server->StartLANDiscovery();
	TestTrue(TEXT("Automatic discovery starts"), Server->bLANDiscoveryActive);
	Server->StopLANDiscovery();
	Server->TickDiscovery(0.25f);
	TestFalse(TEXT("Stopped loop cannot restart"), Server->bSearchingForServers || Server->bLANDiscoveryActive);
	Server->ServerState = EServerState::Hosting;
	Server->StartLANDiscovery();
	TestFalse(TEXT("Hosting cannot start discovery"), Server->bLANDiscoveryActive);
	Server->ServerState = EServerState::Idle;
	Instance->Shutdown();
	Server->OnFindSessionsComplete(true, NewId);
	TestFalse(TEXT("Late callback after teardown is ignored"), Server->bSearchingForServers);
	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	return true;
}
#endif
