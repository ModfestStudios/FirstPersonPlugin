// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AIBehaviorSubsystem.h"

void UAIBehaviorSubsystem::RegisterAIController(AFirstPersonAIController* Controller)
{
	AIControllers.AddUnique(Controller);
}

void UAIBehaviorSubsystem::RegisterSensesComponent(USensesComponent* SensesComponent)
{
	AISensesComponents.AddUnique(SensesComponent);
}
