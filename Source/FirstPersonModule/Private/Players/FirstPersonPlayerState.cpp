// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/FirstPersonPlayerState.h"
#include "Players/FirstPersonPlayerController.h"
#include "GameModes/FirstPersonGameState.h"
#include "GameModes/FirstPersonGame.h"

/*rosters*/
#include "Rosters/RosterInfo.h"


/*network*/
#include "Net/UnrealNetwork.h"

void AFirstPersonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirstPersonPlayerState, bReady);
	DOREPLIFETIME(AFirstPersonPlayerState, bLevelInstanceLoading);
	DOREPLIFETIME_CONDITION(AFirstPersonPlayerState, LevelInstanceLoadPercent, COND_SkipOwner);
}



bool AFirstPersonPlayerState::IsPlayerReady()
{
	return bReady;
}

void AFirstPersonPlayerState::SetPlayerReady(bool bIsReady)
{
	AController* PC = GetOwningController();

	/*need to be server or local client*/
	if (PC && PC->IsLocalPlayerController())
	{
		bReady = bIsReady;

		NotifyReadyStateChanged();

		/*notify server*/
		if (GetNetMode() == NM_Client)
			Server_SetPlayerReady(bIsReady);		
		
	}
}

void AFirstPersonPlayerState::NotifyReadyStateChanged()
{
	/*server only*/
	if (GetNetMode() < NM_Client)
	{
		/*notify gamemode*/
		if (AFirstPersonGame* GM = GetWorld()->GetAuthGameMode<AFirstPersonGame>())
		{
			AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(GetOwningController());
			GM->OnPlayerReadyStateChanged(PC, bReady);
		}
	}

	/*notify gamestate (we notify GS for clients mostly, but we also notify GM so game-play logic can be easily ran from the change*/
	if (AFirstPersonGameState* GS = GetWorld()->GetGameState<AFirstPersonGameState>())
	{
		GS->NativeOnPlayerReadyStateChanged(this, bReady);
	}

	/*notify anyone listening*/
	if (OnReadyStateChanged.IsBound())
		OnReadyStateChanged.Broadcast(this, bReady);
}

void AFirstPersonPlayerState::OnRep_PlayerReadyStateChanged()
{
	/*broadcast notifications*/
	NotifyReadyStateChanged();
}

bool AFirstPersonPlayerState::IsAssignedToRoster()
{
	if(AssignedRoster && AssignedRoster->HasPlayer(this))
		return true;
	else
		return false;
}

bool AFirstPersonPlayerState::Server_SetPlayerReady_Validate(bool bIsReady)
{
	return true;
}

void AFirstPersonPlayerState::Server_SetPlayerReady_Implementation(bool bIsReady)
{
	bReady = bIsReady;
	NotifyReadyStateChanged();
}

void AFirstPersonPlayerState::SetCharacterInfo(ACharacterInfo* NewCharacterInfo)
{
	if (!NewCharacterInfo)
		return;

	CharacterInfo = NewCharacterInfo;
}

void AFirstPersonPlayerState::SetRosterInfo(ARosterInfo* NewRosterInfo)
{
	AssignedRoster = NewRosterInfo;
}
