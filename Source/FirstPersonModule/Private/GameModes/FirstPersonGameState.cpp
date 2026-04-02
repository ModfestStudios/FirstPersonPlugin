// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FirstPersonGameState.h"
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonLevelScript.h"
#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"


/*engine*/
#include "Engine/GameInstance.h"
#include "Engine/World.h"

/*maps*/
#include "Maps/MapAsset.h"

/*network*/
#include "Net/UnrealNetwork.h"

/*players*/
#include "GameFramework/PlayerState.h"

/*subsystems*/
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/CinemaSubsystem.h"
#include "Subsystems/ServerSubsystem.h"
#include "Subsystems/MapSubsystem.h"

/*utilities*/
#include "Engine/LevelStreamingDynamic.h"
#include "TimerManager.h"
//#include "Async/AsyncLoading.h" 
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"



void AFirstPersonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirstPersonGameState, CharacterList);

	DOREPLIFETIME(AFirstPersonGameState, CSS_ViewTarget);

	//DOREPLIFETIME(AFirstPersonGameState, CurrentMap);
	//DOREPLIFETIME(AFirstPersonGameState, NextMap);
	//DOREPLIFETIME(AFirstPersonGameState, MapRotation);
}


void AFirstPersonGameState::SyncCharacterList()
{	
	
	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		for (ACharacterInfo* CharacterInfo : CSS->GetAllCharacters())
		{
			CharacterList.Add(CharacterInfo);
		}
	}
}

void AFirstPersonGameState::OnRep_CharacterListChanged()
{
	/*no need to do this for server*/
	if (HasAuthority())
		return;

	if (UCharacterSubsystem* CSS = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		TArray<ACharacterInfo*> NewCharacterList;
		for (ACharacterInfo* CharacterInfo : CharacterList)
		{			
			NewCharacterList.Add(CharacterInfo);
		}

		CSS->InitCharacters(NewCharacterList);
	}
}



void AFirstPersonGameState::OnRep_CSSViewTargetChanged()
{
	if (UCinemaSubsystem* CSS = GetGameInstance()->GetSubsystem<UCinemaSubsystem>())
	{
		CSS->SetViewTarget(CSS_ViewTarget);
	}
}



//=================================
//==========MAP ROTATIONS==========
//=================================

void AFirstPersonGameState::Multicast_OnCurrentMapUpdated_Implementation(UMapAsset* CurrentMap)
{
	/*notify level script actor of event*/
	if (AFirstPersonLevelScript* LS = GetLevelScript())
		LS->OnCurrentMapChanged(CurrentMap);

	if (!HasAuthority()) //client safety check
	{
		if (UMapSubsystem* MSS = GetMapSubsystem())
			MSS->Client_OnCurrentMapUpdated(CurrentMap);
	}
}

void AFirstPersonGameState::Multicast_OnNextMapUpdated_Implementation(UMapAsset* NextMap)
{
	/*notify level script actor of event*/
	if (AFirstPersonLevelScript* LS = GetLevelScript())
		LS->OnNextMapChanged(NextMap);

	if (!HasAuthority())
	{
		if (UMapSubsystem* MSS = GetMapSubsystem())
			MSS->Client_OnNextMapUpdated(NextMap);
	}
}

void AFirstPersonGameState::Multicast_OnMapRotationUpdated_Implementation(const TArray<class UMapAsset*>& MapRotation)
{
	/*notify level script actor of event*/
	if (AFirstPersonLevelScript* LS = GetLevelScript())
		LS->OnMapRotationChanged(MapRotation);

	if (!HasAuthority()) //client safety check
	{
		if (UMapSubsystem* MSS = GetMapSubsystem())
			MSS->Client_OnMapRotationUpdated(MapRotation);
	}
}


int32 AFirstPersonGameState::GetPlayersLoadingLevelInstanceNum()
{
	int32 PlayersLoading = 0;

	for (APlayerState* Player : PlayerArray)
	{
		AFirstPersonPlayerState* PS = Cast<AFirstPersonPlayerState>(Player);

		if (!PS)
			continue;

		/*count all player states loading*/
		if(PS->bLevelInstanceLoading == true)
			PlayersLoading++;
	}

	return PlayersLoading;
}

/*LoadLevelInstance() - A Multicast (Server & All Clients) trigger to load the passed in MapAsset
* Upon Completion - OnLevelInstanceLoadFinished() will be called locally for each client to relay they've loaded to the server
*
*
*/
void AFirstPersonGameState::Multicast_LoadLevelInstance_Implementation(const FSoftObjectPath& LevelPath, FVector Location, FRotator Rotation)
{
	UE_LOG(LogGameMode, Log, TEXT("%s::Multicast_LoadLevelInstance() - Begin"), *GetName());
	/*update flag*/
	bLoadingLevel = true;
	LoadingLevelPath = LevelPath.GetLongPackageName();

	TSoftObjectPtr<UWorld> LevelAsset(LevelPath);
	UE_LOG(LogGameMode, Log, TEXT("\t Loading Level %s..."),*LevelPath.ToString());

	/*init*/
	bool bSuccess = false;
	//TSoftObjectPtr<UWorld> LevelAsset = MapAsset->Level;
	LoadingLevelStream = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		GetWorld(),
		LevelAsset,
		Location,
		Rotation,
		bSuccess,
		LevelPath.GetAssetName());

	if (bSuccess && LoadingLevelStream)
	{
		UE_LOG(LogGameMode, Log, TEXT("\t Load Success"));
		LoadingLevelStream->SetShouldBeVisible(true);
		LoadingLevelStream->SetShouldBeLoaded(true);

		/*start timer that'll check (locally) if the level has loaded for this player*/
		GetWorld()->GetTimerManager().SetTimer(LevelStreamHandler, this, &AFirstPersonGameState::CheckLevelLoaded, 0.005f, true);
	}
	else
	{
		UE_LOG(LogGameMode, Warning, TEXT("\t LOAD FAILED"));
		bLoadingLevel = false; //failed so cancel load
	}
	
	UE_LOG(LogGameMode, Log, TEXT("%s::Multicast_LoadLevelInstance() - End"), *GetName());
}

void AFirstPersonGameState::CheckLevelLoaded()
{
	if (!LoadingLevelStream)
	{
		return;
	}

	/*check load percentage*/
	AFirstPersonPlayerController* PC = GetWorld()->GetFirstPlayerController<AFirstPersonPlayerController>();
	AFirstPersonPlayerState* PS = PC ? PC->GetPlayerState<AFirstPersonPlayerState>() : nullptr;
	//const FName PackageName = LoadingLevelStream->GetWorldAssetPackageFName();
	float Pct = GetAsyncLoadPercentage(FName(LoadingLevelPath));

	UE_LOG(LogTemp, Log, TEXT("CheckLevelLoaded() - %s - %f"), *LoadingLevelPath, Pct);

	if (LoadingLevelStream->IsLevelLoaded() && LoadingLevelStream->IsLevelVisible())
	{
		/*update flag*/
		bLoadingLevel = false;
		if (PS) //update our local playerstate
			PS->LevelInstanceLoadPercent = 100.0f;
		if (PC && GetNetMode() == NM_Client) //have server update everyone else
			PC->Server_UpdateLevelLoadPercent(100.0f);

		/*clear out timer*/
		GetWorld()->GetTimerManager().ClearTimer(LevelStreamHandler);


		/*send off the notification*/
		OnLevelInstanceLoadFinished();
	}
	else
	{
		/*small clamp for UI purposes since -1 means not loading & 100% doesn't guarantee visibility yet*/
		Pct = FMath::Clamp(Pct, 0.0f, 95.0f);
		

		/*mark percent*/
		if (PS) //update our local playerstate
			PS->LevelInstanceLoadPercent = Pct;
		if (PC && GetNetMode() == NM_Client) //have server update everyone else
			PC->Server_UpdateLevelLoadPercent(Pct);
	}
}

void AFirstPersonGameState::OnLevelInstanceLoadFinished()
{
	UE_LOG(LogGameMode, Log, TEXT("%s::OnLevelInstanceLoadFinished() - Begin"), *GetName());
	
	/*init*/
	AFirstPersonPlayerController* PC = GetWorld()->GetFirstPlayerController<AFirstPersonPlayerController>();
	FString PlayerName = PC ? PC->GetPlayerState<APlayerState>()->GetPlayerName() : FString("Uknown Player");
	int32 PlayerId = PC->GetPlayerState<APlayerState>()->GetPlayerId();
	UE_LOG(LogGameMode, Log, TEXT("\t Client Name: %s Client ID: %s"), *PlayerName, *FString::SanitizeFloat(PlayerId));

	/*trigger Blueprint event*/
	BP_OnLevelInstanceLoadFinished();

	/*notify server that we've completed loading*/
	
	PC->ServerNotify_LevelInstanceLoadFinished();

	UE_LOG(LogGameMode, Log, TEXT("%s::OnLevelInstanceLoadFinished() - End"), *GetName());
}

AFirstPersonLevelScript* AFirstPersonGameState::GetLevelScript()
{
	ULevel* CurrentLevel = GetWorld()->GetCurrentLevel();
	return Cast<AFirstPersonLevelScript>(GetWorld()->GetLevelScriptActor(CurrentLevel));
}

UServerSubsystem* AFirstPersonGameState::GetServerSubsystem()
{
	return GetGameInstance()->GetSubsystem<class UServerSubsystem>();
}

UMapSubsystem* AFirstPersonGameState::GetMapSubsystem()
{
	return GetGameInstance()->GetSubsystem<class UMapSubsystem>();
}





