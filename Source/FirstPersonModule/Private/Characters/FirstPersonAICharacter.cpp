// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FirstPersonAICharacter.h"
#include "AI/FirstPersonAIController.h"

/*components*/
#include "Components/BehaviorComponent.h"
#include "Components/SensesComponent.h"

AFirstPersonAICharacter::AFirstPersonAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	
	AIControllerClass = AFirstPersonAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
