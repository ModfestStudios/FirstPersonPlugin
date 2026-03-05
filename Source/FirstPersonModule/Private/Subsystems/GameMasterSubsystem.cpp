// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameMasterSubsystem.h"

#include "AI/FirstPersonAIController.h"
#include "Characters/FirstPersonCharacter.h"
#include "Players/FirstPersonPlayerController.h"
#include "GameModes/FirstPersonGame.h"
#include "GameModes/FirstPersonGameState.h"
#include "../Logging.h"


bool UGameMasterSubsystem::IsServer()
{
	/*returns true if we're a type of server*/
	return GetWorld()->GetNetMode() < NM_Client;
}
