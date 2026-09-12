// Copyrighted : Modfest Studios 2025-2026


#include "AI/Services/BTService_EvaluateTargets.h"
#include "AI/FirstPersonAIController.h"
#include "Components/BehaviorComponent.h"

UBTService_EvaluateTargets::UBTService_EvaluateTargets()
{
	NodeName = "Evaluate Targets";
	bNotifyTick = true;
	bCreateNodeInstance = false;
}

void UBTService_EvaluateTargets::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());	
	UBehaviorComponent* BehaviorComponent = AIController ? AIController->GetPawnBehaviorComponent() : nullptr;

	if (!AIController || !BehaviorComponent)
		return;

	BehaviorComponent->EvaluateTargets(TargetProfile, DeltaSeconds);
}

void UBTService_EvaluateTargets::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	
}

void UBTService_EvaluateTargets::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

}

void UBTService_EvaluateTargets::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	
}
