// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AIBehaviorSubsystem.h"

void UAIBehaviorSubsystem::RegisterActor(AActor* Actor)
{
	RegisteredActors.Add(Actor);
}

void UAIBehaviorSubsystem::UnregisterActor(AActor* Actor)
{
	RegisteredActors.Remove(Actor);
}

const TSet<AActor*>& UAIBehaviorSubsystem::GetRegisteredActors() const
{
	return RegisteredActors;
}

void UAIBehaviorSubsystem::RegisterAIController(AFirstPersonAIController* Controller)
{
	AIControllers.AddUnique(Controller);
}

void UAIBehaviorSubsystem::RegisterSensesComponent(USensesComponent* SensesComponent)
{
	AISensesComponents.AddUnique(SensesComponent);
}
