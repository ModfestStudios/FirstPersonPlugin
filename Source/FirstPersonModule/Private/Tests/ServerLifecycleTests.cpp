// Copyright Modfest Studios
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Online/ServerSettings.h"
#include "Subsystems/ServerSubsystem.h"
#include "UObject/StrongObjectPtr.h"

//========================================================FUNCTIONS========================================================
//=====================SESSION LIFECYCLE=====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FServerLifecycleAutomationTest, "FirstPersonModule.Server.TransportAndRecovery", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FServerLifecycleAutomationTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UGameInstance> Instance(NewObject<UGameInstance>(GEngine));
	TStrongObjectPtr<UGameInstance> OtherInstance(NewObject<UGameInstance>(GEngine));
	UServerSubsystem* Server;
	UServerSubsystem* OtherServer;
	UWorld* World;
	UWorld* OtherWorld;
	TArray<FNetDriverDefinition> OriginalDefinitions = GEngine->NetDriverDefinitions;
	FGuid RequestId;
	FOnlineSessionSettings SessionSettings;
	FName SelectedDriver;

	Instance->InitializeStandalone();
	OtherInstance->InitializeStandalone();
	World = Instance->GetWorld();
	OtherWorld = OtherInstance->GetWorld();
	Server = Instance->GetSubsystem<UServerSubsystem>();
	OtherServer = OtherInstance->GetSubsystem<UServerSubsystem>();

	// Selecting LAN/direct must override a Steam default only for our pending travel.
	for (FNetDriverDefinition& Definition : GEngine->NetDriverDefinitions)
	{
		if (Definition.DefName == NAME_GameNetDriver)
			Definition.DriverClassName = TEXT("/Script/SteamSockets.SteamSocketsNetDriver");
	}
	TestTrue(TEXT("LAN can select IP despite a Steam default"), Server->AcquireTransport(EServerConnectionType::LAN));
	for (const FNetDriverDefinition& Definition : GEngine->NetDriverDefinitions)
	{
		if (Definition.DefName == NAME_GameNetDriver)
			SelectedDriver = Definition.DriverClassName;
	}
	TestEqual(TEXT("LAN explicitly selects platform IP transport"), SelectedDriver, FName(TEXT("/Script/OnlineSubsystemUtils.IpNetDriver")));
	TestFalse(TEXT("Another PIE instance cannot overwrite pending transport"), OtherServer->AcquireTransport(EServerConnectionType::Direct));
	Server->ReleaseTransport();
	for (const FNetDriverDefinition& Definition : GEngine->NetDriverDefinitions)
	{
		if (Definition.DefName == NAME_GameNetDriver)
			SelectedDriver = Definition.DriverClassName;
	}
	TestEqual(TEXT("Configured Steam driver is restored after lease"), SelectedDriver, FName(TEXT("/Script/SteamSockets.SteamSocketsNetDriver")));
	TestTrue(TEXT("Next instance can acquire IP after release"), OtherServer->AcquireTransport(EServerConnectionType::Direct));
	OtherServer->ReleaseTransport();
	GEngine->NetDriverDefinitions = OriginalDefinitions;

	// Initial hosting is a fresh absolute listen load, even if the menu GameMode
	// enables seamless travel. No existing network driver is available to preserve.
	Server->BeginSessionOperation(FGuid::NewGuid(), EServerOperation::Host, EServerConnectionType::Direct);
	Server->BeginSessionTravel(TEXT("/Engine/Maps/Entry"));
	TestTrue(TEXT("Initial host explicitly requests listen travel"), GEngine->GetWorldContextFromWorldChecked(World).TravelURL.Contains(TEXT("?listen")));
	TestEqual(TEXT("Host waits for its new listen world"), Server->ServerState, EServerState::TravelPending);
	GEngine->GetWorldContextFromWorldChecked(World).TravelURL.Empty();

	// Merely resolving a shared provider must not grant cleanup ownership. An idle
	// second instance leaving must not delete the first instance's named session.
	OtherServer->SessionInterface = OtherServer->ResolveLANInterface();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 32;
	OtherServer->SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	OtherServer->LeaveSession();
	TestNotNull(TEXT("An unowned session survives another instance leaving"), Server->ResolveLANInterface()->GetNamedSession(NAME_GameSession));
	Server->ResolveLANInterface()->DestroySession(NAME_GameSession);
	GEngine->GetWorldContextFromWorldChecked(OtherWorld).TravelURL.Empty();
	OtherServer->bReturningToMenu = false;

	// Destroy an actual Null session through the public LeaveSession path.
	Server->SessionInterface = Server->ResolveLANInterface();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 32;
	TestTrue(TEXT("Null session created for cleanup test"), Server->SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings));
	Server->BeginSessionOperation(FGuid::NewGuid(), EServerOperation::Host, EServerConnectionType::LAN);
	Server->SetServerState(EServerState::Hosting);
	Server->LeaveSession();
	TestNull(TEXT("Leaving destroys the named provider session"), Server->ResolveLANInterface()->GetNamedSession(NAME_GameSession));
	TestTrue(TEXT("Leave waits for standalone menu travel"), Server->bReturningToMenu);
	TestEqual(TEXT("Cleanup does not prematurely publish Idle"), Server->ServerState, EServerState::Leaving);
	GEngine->GetWorldContextFromWorldChecked(World).TravelURL.Empty();
	Server->bReturningToMenu = false;

	// A travel timeout must release transport and schedule recovery, not report success.
	RequestId = FGuid::NewGuid();
	Server->BeginSessionOperation(RequestId, EServerOperation::Join, EServerConnectionType::Direct);
	Server->SetServerState(EServerState::TravelPending);
	Server->OperationStartedAt = FPlatformTime::Seconds() - GetDefault<UServerSettings>()->OperationTimeoutSeconds - 1.0;
	Server->TickDiscovery(0.25f);
	TestTrue(TEXT("Timeout reports the operation outcome"), Server->bOperationReported);
	TestTrue(TEXT("Timeout schedules return to menu"), Server->bReturningToMenu);
	TestEqual(TEXT("Recovery preserves timed-out request identity"), Server->SessionRequestId, RequestId);
	GEngine->GetWorldContextFromWorldChecked(World).TravelURL.Empty();
	Server->bReturningToMenu = false;

	// A late create must not start travel or replace a newer operation.
	Server->BeginSessionOperation(FGuid::NewGuid(), EServerOperation::Host, EServerConnectionType::LAN);
	Server->SetServerState(EServerState::Creating);
	Server->OnCreateSessionComplete(NAME_GameSession, true, RequestId);
	TestEqual(TEXT("Stale create cannot change current state"), Server->ServerState, EServerState::Creating);
	TestTrue(TEXT("Stale create cannot schedule travel"), GEngine->GetWorldContextFromWorldChecked(World).TravelURL.IsEmpty());

	// Leave during an uncancellable provider create keeps the old callback identity,
	// then destroys the result under a distinct leave request instead of traveling.
	Server->SessionInterface = Server->ResolveLANInterface();
	RequestId = Server->SessionRequestId;
	Server->CreateDelegateHandle = Server->SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(Server, &UServerSubsystem::OnCreateSessionComplete, RequestId));
	Server->LeaveSession();
	TestEqual(TEXT("Leave waits for in-flight create"), Server->ServerState, EServerState::Recovering);
	TestEqual(TEXT("Provider callback retains cancelled request ID"), Server->SessionRequestId, RequestId);
	Server->OnCreateSessionComplete(NAME_GameSession, true, RequestId);
	TestTrue(TEXT("Leave gets its own ID after cancelled callback"), Server->SessionRequestId != RequestId);
	TestTrue(TEXT("Cancelled create returns to menu, never hosts"), Server->bReturningToMenu && Server->ServerState == EServerState::Leaving);
	GEngine->GetWorldContextFromWorldChecked(World).TravelURL.Empty();
	Server->bReturningToMenu = false;

	// Model an outstanding destroy callback; failure must preserve the interface for
	// a retry, rather than pretending the backend lobby was successfully removed.
	Server->SessionInterface = Server->ResolveLANInterface();
	Server->DestroyDelegateHandle = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
	Server->OnDestroySessionComplete(NAME_GameSession, false, Server->SessionRequestId);
	TestEqual(TEXT("Destroy failure blocks a new session"), Server->ServerState, EServerState::RecoveryFailed);
	TestTrue(TEXT("Failed cleanup retains provider for retry"), Server->SessionInterface.IsValid());
	TestFalse(TEXT("Completed destroy releases callback ownership"), Server->DestroyDelegateHandle.IsValid());

	OtherInstance->Shutdown();
	Instance->Shutdown();
	OtherWorld->DestroyWorld(false);
	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(OtherWorld);
	GEngine->DestroyWorldContext(World);

	return true;
}
#endif
