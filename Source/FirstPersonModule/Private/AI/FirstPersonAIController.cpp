// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FirstPersonAIController.h"

/*engine*/
#include "Engine/World.h"

/*subsystems*/
#include "Subsystems/GameMasterSubsystem.h"
#include "Subsystems/AIBehaviorSubsystem.h"

AFirstPersonAIController::AFirstPersonAIController()
{

}

void AFirstPersonAIController::BeginPlay()
{
	if (GetWorld())
	{
		GetWorld()->GetSubsystem<UAIBehaviorSubsystem>()->RegisterAIController(this);
	}
}

void AFirstPersonAIController::SetTarget(AActor* NewTarget)
{
	/*cancel if invalid*/
	if (!IsValid(NewTarget))
		return;

	/*mark our last known target*/
	if(CurrentTarget != nullptr)
		PreviousTarget = CurrentTarget;

	/*set target to new one*/
	CurrentTarget = NewTarget;
}

AActor* AFirstPersonAIController::GetClosestThreat()
{
	return nullptr;
}

AActor* AFirstPersonAIController::GetHighestThreat()
{
	return nullptr;
}
