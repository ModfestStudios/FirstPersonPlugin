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
	if (SetTargetTo == ESetTargetType::LowestThreat)
	{
		NewTarget = AIController->GetLowestThreat();
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

		if (SetTargetTo != ESetTargetType::BlackboardKey && bSetTargetToBlackboardKey)
		{
			if(UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
				BlackboardComp->SetValueAsObject(BlackboardKey.SelectedKeyName,NewTarget);
		}

		return EBTNodeResult::Succeeded;
	}
	else
		return EBTNodeResult::Failed;
	
}

FString UBTTask_SetTarget::GetStaticDescription() const
{
	if(SetTargetTo == ESetTargetType::ClosestThreat)
		return FString("Closest Threat");
	if(SetTargetTo == ESetTargetType::HighestThreat)
		return FString("Highest Threat");
	if (SetTargetTo == ESetTargetType::LowestThreat)
		return FString("Lowest Threat");
	if(SetTargetTo == ESetTargetType::BlackboardKey)
		return FString("Blackboard Key");


	return FString();
}
