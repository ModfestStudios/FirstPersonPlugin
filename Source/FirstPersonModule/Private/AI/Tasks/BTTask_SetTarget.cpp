// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SetTarget.h"
#include "AI/FirstPersonAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_SetTarget::UBTTask_SetTarget()
{
	NodeName = "Set Target";
}

EBTNodeResult::Type UBTTask_SetTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());

	if (!AIController)
		return EBTNodeResult::Failed;

	AActor* NewTarget = nullptr;

	if (SetTargetTo == ESetTargetType::ClosestThreat)
	{
		NewTarget = AIController->GetClosestThreat();
	}
	if (SetTargetTo == ESetTargetType::HighestThreat)
	{
		NewTarget = AIController->GetHighestThreat();
	}
	if (SetTargetTo == ESetTargetType::BlackboardKey)
	{
		if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
			NewTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardKey.SelectedKeyName));					
	}

	/*set the target*/
	if (NewTarget)
	{
		AIController->SetTarget(NewTarget);
		return EBTNodeResult::Succeeded;
	}
	else
		return EBTNodeResult::Failed;
	
}

FString UBTTask_SetTarget::GetStaticDescription() const
{
	
	return FString();
}
