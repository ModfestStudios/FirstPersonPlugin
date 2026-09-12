// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Combat/Combat.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Attack")
		class UAttackAsset* Attack;

	/*whether we should look at the AI Controller's Target or against a specific Blackboard Key*/
	UPROPERTY(EditAnywhere, Category = "Target")
		ECheckTargetType Target = ECheckTargetType::AIController;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "Target == ECheckTargetType::BlackboardKey"))
		FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "Target == ECheckTargetType::BlackboardKey"))
		TEnumAsByte<EBasicKeyOperation::Type> BasicOperation = EBasicKeyOperation::Set;




//=============================================================================================================================================================================================================================================
//==================================================================================================================FUNCTIONS==================================================================================================================
//=============================================================================================================================================================================================================================================


public:
	UBTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;






	
};
