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


	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	USensesComponent* SensesComponent = AIController ? AIController->GetPawnSensesComponent() : nullptr;

	if (!AIController || !SensesComponent)
		return;

	SensesComponent->EvaluateSenses(SensesProfile, DeltaSeconds);
}

void UBTService_EvaluateSenses::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{

}

void UBTService_EvaluateSenses::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
}

void UBTService_EvaluateSenses::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	USensesComponent* SensesComponent = AIController ? AIController->GetPawnSensesComponent() : nullptr;

	if (!AIController || !SensesComponent)
		return;

	int32 NumActorsSensed = SensesComponent->GetNumOfActorsSensing();
	FString RuntimeString = FString("Actors Being Sensed: ") + FString::FromInt(NumActorsSensed);

	Values.Add(RuntimeString);
}

