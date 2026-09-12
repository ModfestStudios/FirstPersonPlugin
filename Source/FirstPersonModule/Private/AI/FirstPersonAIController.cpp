// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FirstPersonAIController.h"

/*character*/
#include "Characters/FirstPersonCharacter.h"

/*components*/
#include "Components/BehaviorComponent.h"

/*engine*/
#include "Engine/World.h"

/*subsystems*/
#include "Subsystems/GameMasterSubsystem.h"
#include "Subsystems/AIBehaviorSubsystem.h"

AFirstPersonAIController::AFirstPersonAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void AFirstPersonAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		GetWorld()->GetSubsystem<UAIBehaviorSubsystem>()->RegisterAIController(this);
	}
}

void AFirstPersonAIController::Attack(UAttackAsset* Attack)
{
	/*safety check*/
	if(!Attack)
		return;

	/*forward request to Character*/
	if (AFirstPersonCharacter* Char = GetFirstPersonCharacter())
	{
		Char->Attack(Attack);
	}
}

//===========================
//==========TARGETS==========
//===========================

bool AFirstPersonAIController::HasTarget()
{
	return IsValid(CurrentTarget);
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

AActor* AFirstPersonAIController::GetTarget() const
{
	return CurrentTarget;
}

AActor* AFirstPersonAIController::GetClosestThreat()
{
	if(AFirstPersonCharacter* Char = GetPawn<AFirstPersonCharacter>())
		if(UBehaviorComponent* BehaviorComponent = Char->GetBehaviorComponent())
			return BehaviorComponent->GetClosestThreat();

	
		return nullptr;
}

//=========================
//=========THREATS=========
//=========================

AActor* AFirstPersonAIController::GetHighestThreat()
{
	if (AFirstPersonCharacter* Char = GetPawn<AFirstPersonCharacter>())
		if (UBehaviorComponent* BehaviorComponent = Char->GetBehaviorComponent())
			return BehaviorComponent->GetHighestThreat();


	return nullptr;
}

AActor* AFirstPersonAIController::GetLowestThreat()
{
	if (AFirstPersonCharacter* Char = GetPawn<AFirstPersonCharacter>())
		if (UBehaviorComponent* BehaviorComponent = Char->GetBehaviorComponent())
			return BehaviorComponent->GetLowestThreat();


	return nullptr;
}

//====================
//========PAWN========	
//====================

AFirstPersonCharacter* AFirstPersonAIController::GetFirstPersonCharacter()
{
	return GetPawn<AFirstPersonCharacter>();
}

USensesComponent* AFirstPersonAIController::GetPawnSensesComponent()
{
	if (AFirstPersonCharacter* Char = GetPawn<AFirstPersonCharacter>())
		return Char->GetSensesComponent();

	return nullptr;
}

UBehaviorComponent* AFirstPersonAIController::GetPawnBehaviorComponent()
{
	if (AFirstPersonCharacter* Char = GetPawn<AFirstPersonCharacter>())
		return Char->GetBehaviorComponent();

	return nullptr;
}
