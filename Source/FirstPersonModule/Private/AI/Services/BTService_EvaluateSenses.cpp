// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_EvaluateSenses.h"
#include "AI/FirstPersonAIController.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/SensesComponent.h"
#include "Subsystems/GameMasterSubsystem.h"
#include "Subsystems/CharacterSubsystem.h"

/*curves*/
#include "Curves/CurveFloat.h"

/*engine*/
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UBTService_EvaluateSenses::UBTService_EvaluateSenses()
{
	NodeName = "Evaluate Senses";
	bNotifyTick = true;
	bCreateNodeInstance = false;
}

void UBTService_EvaluateSenses::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (ShouldUpdateEvaluationList())
		UpdateEvaluationList();
}

void UBTService_EvaluateSenses::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{

}

void UBTService_EvaluateSenses::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
}

USensesComponent* UBTService_EvaluateSenses::GetOwnerSensesComponent(AAIController* AIController)
{
	if (SensesComponent == nullptr)
	{
		if (AIController)
		{
			if (APawn* Pawn = AIController->GetPawn())
				SensesComponent = Cast<USensesComponent>(Pawn->GetComponentByClass(USensesComponent::StaticClass()));
		}
	}

	return SensesComponent;
}

float UBTService_EvaluateSenses::GetDistanceFromActor(UBehaviorTreeComponent& OwnerComp, AActor* Target)
{
	float TargetDistance = 0.0f;

	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (APawn* Pawn = AIController->GetPawn())
		{
			TargetDistance = FVector::Dist(Target->GetActorLocation(), Pawn->GetActorLocation());
		}
	}
	
	return TargetDistance;
}


/*GetActorsToEvaluate() - will return a list of all the actors we wish to evaluate */
TArray<AActor*> UBTService_EvaluateSenses::GetActorsToEvaluate()
{
	TArray<AActor*> Actors;

	if (UCharacterSubsystem* CSS = GetWorld()->GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		for (AActor* PlayerPawn : CSS->GetAllPlayerPawns())
		{
			Actors.AddUnique(PlayerPawn);
		}
		
			//Actors.Append(GM->GetAllPlayerPawns()); //add the Player pawns
	}

	return Actors;
}

bool UBTService_EvaluateSenses::ShouldUpdateEvaluationList()
{
	if ((GetWorld()->TimeSeconds - LastCacheUpdate) >= CacheUpdateRate)
		return true;


	return false;
}

void UBTService_EvaluateSenses::UpdateEvaluationList()
{
	if (GetWorld() == nullptr)
		return;

	LastCacheUpdate = GetWorld()->TimeSeconds;
	TArray<AActor*> NewActorList = GetActorsToEvaluate();	

	for (AActor* Actor : NewActorList)
	{
		if (IsEvaluatingActor(Actor) == false)
		{
			FActorEvaluation ActorEvaluation;
			ActorEvaluation.Actor = Actor;

			ActorsToEvaluate.Add(ActorEvaluation);
		}		
	}
}

bool UBTService_EvaluateSenses::IsEvaluatingActor(AActor* Actor)
{
	for (FActorEvaluation Eval : ActorsToEvaluate)
	{
		if (Eval.Actor == Actor)
			return true;
	}

	return false;
}
