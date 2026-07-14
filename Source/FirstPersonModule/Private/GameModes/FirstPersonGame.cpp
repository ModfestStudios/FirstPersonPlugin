// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"
#include "GameModes/FirstPersonLevelScript.h"
#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"
#include "Rosters/RosterInfo.h"

/*engine*/
#include "Engine/GameInstance.h"

/*inventory*/
#include "Inventory/InventoryLoadout.h"

/*maps*/
#include "Maps/MapAsset.h"

/*subsystems*/
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/MissionSubsystem.h"


/*ui*/
#include "UMG.h"
#include "UI/Widgets/GameLobbyWidget.h"

/*utilities*/
#include "UObject/ConstructorHelpers.h"
#include "../Logging.h"



AFirstPersonGame::AFirstPersonGame()
{
	DefaultPawnClass = AFirstPersonCharacter::StaticClass();
	PlayerControllerClass = AFirstPersonPlayerController::StaticClass();
	PlayerStateClass = AFirstPersonPlayerState::StaticClass();
	GameStateClass = AFirstPersonGameState::StaticClass();
	InventoryLoadoutClass = AInventoryLoadout::StaticClass();

	bUseSeamlessTravel = true;

	static ConstructorHelpers::FClassFinder<UUserWidget>  DefaultEscapeMenuWidgetRef(TEXT("/FirstPersonModule/UI/Blueprints/BP_DefaultEscapeMenu"));
	if (DefaultEscapeMenuWidgetRef.Succeeded())
		EscapeMenuWidgetClass = DefaultEscapeMenuWidgetRef.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget>  DefaultServerWidgetRef(TEXT("/FirstPersonModule/UI/Blueprints/BP_DefaultServerInfo"));
	if (DefaultServerWidgetRef.Succeeded())
		ServerInfoWidgetClass = DefaultServerWidgetRef.Class;

}

void AFirstPersonGame::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	InitSpectatorLocations();
}

void AFirstPersonGame::InitGameState()
{
	Super::InitGameState();
}

void AFirstPersonGame::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* AFirstPersonGame::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AFirstPersonGame::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(NewPlayer))
	{
		PC->InitializeEscapeMenuWidget(EscapeMenuWidgetClass);
		PC->InitializeServerInfoWidget(ServerInfoWidgetClass);

		if (AFirstPersonPlayerState* PS = PC->GetPlayerState<AFirstPersonPlayerState>())
		{
			/*creates the Loadout for the player*/
			if (bInitializePlayerLoadoutOnLogin == true)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = PC;
				TSubclassOf<AInventoryLoadout> LoadoutClass = IsValid(InventoryLoadoutClass) ? InventoryLoadoutClass.Get() : AInventoryLoadout::StaticClass();

				AInventoryLoadout* PlayerLoadout = GetWorld()->SpawnActor<AInventoryLoadout>(LoadoutClass,SpawnParams);

				if(PlayerLoadout)
					PS->SetLoadout(PlayerLoadout);
			}
		}
	}
}

void AFirstPersonGame::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AFirstPersonGame::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	//Super::HandleStartingNewPlayer_Implementation(NewPlayer); //we're overriding the parent call entirely

	AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(NewPlayer);

	/*safety check*/
	if (!NewPlayer)
		return;



	/*if the player should have a pawn immedietely - spawn them*/
	if (!bStartPlayersAsSpectators && !MustSpectate(NewPlayer) && PlayerCanRestart(NewPlayer))
	{
		///*create a default character info for player*/
		//ACharacterInfo* CharInfo = SpawnCharacterInfoForPlayer(NewPlayer);

		//if (AFirstPersonPlayerState* PlayerState = NewPlayer->GetPlayerState<AFirstPersonPlayerState>())
		//{
		//	PlayerState->SetCharacterInfo(CharInfo);
		//}

		// Otherwise spawn their pawn immediately
		RestartPlayer(NewPlayer);

		/*clear any UI restrictions on spawn*/
		NewPlayer->SetInputMode(FInputModeGameOnly::FInputModeGameOnly());
		NewPlayer->bShowMouseCursor = false;
	}
	if (bStartPlayersAsSpectators && bStartPlayersWithSpectatorPawn)
	{
		SpawnAndPossessSpectatorPawn(PC);
	}
	else
	{	
		
	}

	if (bShowLobbyOnJoin && LobbyWidgetClass)
	{
		PC->InitializeLobbyWidget(LobbyWidgetClass, true);
	}
}

void AFirstPersonGame::RestartPlayer(AController* NewPlayer)
{
	/*spawn the player as normal*/
	Super::RestartPlayer(NewPlayer);

	/*initialize first person character pawn*/
	if (AFirstPersonCharacter* PlayerPawn = Cast<AFirstPersonCharacter>(NewPlayer->GetPawn()))
	{	
			
		//PlayerPawn->
	}

}

void AFirstPersonGame::OnPlayerReadyStateChanged(AFirstPersonPlayerController* Player, bool bReady)
{	
	if(AllPlayersReady())
		OnAllPlayersReadied();

	/*call blueprint version*/
	BP_OnPlayerReadyStateChanged(Player, bReady);
}

void AFirstPersonGame::OnAllPlayersReadied()
{

	/*call blueprint version*/
	BP_OnAllPlayersReadied();
}

bool AFirstPersonGame::AllPlayersReady()
{	
	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
	{
		return GS->AllPlayersReady();
	}

	/*no game-state available yet, so players are not ready*/
	else
		return false;
}


int32 AFirstPersonGame::GetNumPlayers()
{
	return Super::GetNumPlayers();
	//return int32();
}

int32 AFirstPersonGame::GetNumSpectators()
{
	return Super::GetNumSpectators();
	//return int32();
}

TArray<class AFirstPersonPlayerController*> AFirstPersonGame::GetAllPlayers()
{
	TArray<AFirstPersonPlayerController*> PlayerControllers;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(Iterator->Get());

		if (IsValid(PC))
			PlayerControllers.AddUnique(PC);
	}

	return PlayerControllers;
}



bool AFirstPersonGame::AllPlayersDead()
{	
	for (AFirstPersonPlayerController* PC : GetAllPlayers())
	{
		if (AFirstPersonCharacter* Pawn = PC->GetPawn<AFirstPersonCharacter>())
		{
			if (Pawn->IsPlayerDead() == false)
				return false;
		}
	}

	return true;
}

void AFirstPersonGame::OnPlayerDeath(AFirstPersonCharacter* Character, AActor* DamageCauser, AController* DamageInstigator, const UDamageType* DamageType)
{
	/*call blueprint event*/
	BP_OnPlayerDeath(Character, DamageCauser, DamageInstigator, DamageType);

	/*notify level script*/
	if (AFirstPersonLevelScript* LSA = GetLevelScript<AFirstPersonLevelScript>(GetWorld()->GetCurrentLevel()))
	{
		LSA->OnPlayerDeath(Character, DamageCauser, DamageInstigator, DamageType);
	}	

	/*notify Level Instance*/
	if (ULevelStreamingDynamic* LevelStream = GetLevelInstance())
	{
		if (LevelStream->IsLevelLoaded())
		{
			if (AFirstPersonLevelScript* LSA = GetLevelScript<AFirstPersonLevelScript>(LevelStream->GetLoadedLevel()))
				LSA->OnPlayerDeath(Character, DamageCauser, DamageInstigator, DamageType);
		}
	}
}

//==========================
//========SPECTATORS========
//==========================

void AFirstPersonGame::InitSpectatorLocations()
{
	SpectatorSpawnLocations.Empty();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* FoundActor = nullptr;
		if (It->ActorHasTag(SpectatorSpawnTag))
		{
			FoundActor = *It;

			if (FoundActor)
				SpectatorSpawnLocations.AddUnique(FoundActor);
		}
	}
}

AActor* AFirstPersonGame::FindSpectatorSpawnLocation(AFirstPersonPlayerController* Player)
{
	if (SpectatorSpawnLocations.Num() < 1)
		return nullptr;

	int32 RandIndex = FMath::RandRange(0, SpectatorSpawnLocations.Num() - 1);
	AActor* SpectatorSpawn = SpectatorSpawnLocations[RandIndex];

	return SpectatorSpawn;
}

void AFirstPersonGame::MoveSpectatorToLocation(AFirstPersonPlayerController* Player, AActor* LocationRef)
{
	if (Player && LocationRef)
	{
		if (APawn* PlayerPawn = Player->GetPawn())
		{
			PlayerPawn->TeleportTo(LocationRef->GetActorLocation(), LocationRef->GetActorRotation(), false, true);
			Player->SetControlRotation(LocationRef->GetActorRotation());
		}
		else
		{
			FViewTargetTransitionParams TransitionParams;			
			Player->SetViewTarget(LocationRef, TransitionParams);
		}
	}	
}

void AFirstPersonGame::SpawnAndPossessSpectatorPawn(AFirstPersonPlayerController* Player)
{
	/*if they already have a pawn - then bail out*/
	if (!Player || Player->GetPawn())
	{
		return;
	}

	/*init*/
	AActor* SpawnPoint = bUseSpectatorSpawns ? FindSpectatorSpawnLocation(Player) : FindPlayerStart(Player);
	const FTransform SpawnTransform = SpawnPoint ? SpawnPoint->GetActorTransform() : FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	/*spawn*/
	APawn* SpectatorPawn = GetWorld()->SpawnActor<APawn>(SpectatorClass,SpawnTransform,SpawnParams);

	if (!SpectatorPawn)
	{
		return;
	}

	Player->Possess(SpectatorPawn);
	Player->SetControlRotation(SpawnTransform.Rotator());
	Player->ClientSetRotation(SpawnTransform.Rotator(), true);
}



ACharacterInfo* AFirstPersonGame::SpawnCharacterInfoForPlayer(APlayerController* Player)
{
	ACharacterInfo* CharacterInfo = nullptr;
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		CharacterInfo = CSS->CreateCharacterInfo(Player);

	return CharacterInfo;
}



void AFirstPersonGame::OnCurrentMapUpdated(UMapAsset* MapAsset)
{
	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
		GS->Multicast_OnCurrentMapUpdated(MapAsset);
}

/*OnNextMapSet() - Event called from MapSubsystem whenever the "Next Map" (pending map to switch to) is set*/
void AFirstPersonGame::OnNextMapUpdated(UMapAsset* MapAsset)
{
	/*notify GameState to replicate to other players*/
	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
		GS->Multicast_OnNextMapUpdated(MapAsset);
}

void AFirstPersonGame::OnMapRotationUpdated(TArray<class UMapAsset*> MapRotation)
{
	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
		GS->Multicast_OnMapRotationUpdated(MapRotation);
}

/*GetSeamlessTravelActorList() - Generates a list of actors that should be transferred from one map to another
* Preserves things such the Team Roster setup, Character Selection, and Loadouts
*
*
*
*
* NOTE: Does not get called if using Play In Editor (PIE) - need to do Standalone Game to trigger
*/
void AFirstPersonGame::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	UE_LOG(LogFirstPersonGameMode, Log, TEXT("%s::GetSeamlessTravelActorList()"),*GetName());

	/*keep all team roster info & their members*/
	if (UMissionSubsystem* MSS = GetGameInstance()->GetSubsystem<UMissionSubsystem>())
	{
		UE_LOG(LogFirstPersonGameMode, Log, TEXT("Getting Mission Persistent Actors"));

		for (ARosterInfo* TeamRoster : MSS->GetTeamRosters())
		{
			UE_LOG(LogFirstPersonGameMode, Log, TEXT("adding Team Roster: %s"),*TeamRoster->TeamName);

			ActorList.Add(TeamRoster);

			for (ACharacterInfo* TeamMember : TeamRoster->GetTeamMembers())
			{
				UE_LOG(LogFirstPersonGameMode, Log, TEXT("\t adding Team Member (%s): %s"),*TeamRoster->TeamName,*TeamMember->GetFullName());

				ActorList.Add(TeamMember);
			}
		}
	}

	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}

ULevelStreamingDynamic* AFirstPersonGame::GetLevelInstance()
{
	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
		return GS->LoadingLevelStream;
	else
		return nullptr;
}

bool AFirstPersonGame::HasLevelInstanceLoadedForAllPlayers()
{
	UWorld* World = GetWorld();
	
	if (World)
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Iterator->Get();
			AFirstPersonPlayerState* PS = PC ? PC->GetPlayerState<AFirstPersonPlayerState>() : nullptr;

			if (PS && PS->bLevelInstanceLoading == true) //if a single player is still loading - return false
				return false;
		}
	}

	return true;
}

void AFirstPersonGame::LoadDynamicLevelInstanceForAllPlayers(UMapAsset* MapAsset, FVector Location, FRotator Rotation)
{
	UE_LOG(LogGameMode, Log, TEXT("%s::LoadDynamicLevelInstanceForAllPlayers()"), *GetName());

	if (AFirstPersonGameState* GS = GetGameState<AFirstPersonGameState>())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Iterator->Get();
			AFirstPersonPlayerState* PS = PC ? PC->GetPlayerState<AFirstPersonPlayerState>() : nullptr;

			if (PS)
			{
				UE_LOG(LogGameMode, Log, TEXT("\t player %s has begun Loading Level Instance"), *PS->GetName());
				PS->bLevelInstanceLoading = true; //flag our PlayerState as loading
				PS->LevelInstanceLoadPercent = 0.0f; //reset load progress
			}
		}

		if (MapAsset->Level.IsNull())
		{
			UE_LOG(LogGameMode, Warning, TEXT("\t NO VALID LEVEL FOUND IN MAPASSET %s"), *GetNameSafe(MapAsset));
			return;
		}

		/*init*/
		FSoftObjectPath LevelPath = MapAsset->Level.ToSoftObjectPath();
		GS->Multicast_LoadLevelInstance(LevelPath, Location, Rotation);
	}
}

void AFirstPersonGame::OnPlayerLevelInstanceLoaded(APlayerController* Player)
{
	UE_LOG(LogGameMode, Log, TEXT("%s::OnPlayerLevelInstanceLoaded() - Begin"), *GetName());
	AFirstPersonPlayerState* PS = Player->GetPlayerState<AFirstPersonPlayerState>();
	if (PS)
	{
		UE_LOG(LogGameMode, Log, TEXT("\t player %s has finished Loading Level Instance"), *PS->GetName());
		PS->bLevelInstanceLoading = false; //flag our Player as finished loading
	}		

	/*additional hook for designers*/
	if (HasLevelInstanceLoadedForAllPlayers())
		OnLevelInstanceLoadedForAllPlayers();

	/*call BP hook*/
	BP_OnPlayerLevelInstanceLoaded(Player);

	UE_LOG(LogGameMode, Log, TEXT("%s::OnPlayerLevelInstanceLoaded() - End"), *GetName());
}

void AFirstPersonGame::OnLevelInstanceLoadedForAllPlayers()
{
	UE_LOG(LogGameMode, Log, TEXT("%s::OnLevelInstanceLoadedForAllPlayers()"), *GetName());
	/*call BP hook*/
	BP_OnLevelInstanceLoadedForAllPlayers();
}
