// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FirstPersonAICharacter.h"
#include "AI/FirstPersonAIController.h"

/*components*/
#include "Components/BehaviorComponent.h"
#include "Components/SensesComponent.h"

AFirstPersonAICharacter::AFirstPersonAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	BehaviorComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorComponent>(this, FName("Behavior Component"));
	if (BehaviorComponent)
	{

	}

	SensesComponent = ObjectInitializer.CreateDefaultSubobject<USensesComponent>(this, FName("Senses Component"));
	if (SensesComponent)
	{

	}
	
	AIControllerClass = AFirstPersonAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
