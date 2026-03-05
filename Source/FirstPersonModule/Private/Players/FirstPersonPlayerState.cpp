// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/FirstPersonPlayerState.h"


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
	if (bIsReady != bReady) //prevents duplicate calls
	{
		bReady = bIsReady;
		
		if (OnReadyStateChanged.IsBound())
			OnReadyStateChanged.Broadcast(this, bReady);
	}	
}

void AFirstPersonPlayerState::SetCharacterInfo(ACharacterInfo* NewCharacterInfo)
{
	if (!NewCharacterInfo)
		return;

	CharacterInfo = NewCharacterInfo;
}
