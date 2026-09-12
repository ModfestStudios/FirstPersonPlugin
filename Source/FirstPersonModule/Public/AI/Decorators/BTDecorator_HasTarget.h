// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasTarget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTDecorator_HasTarget : public UBTDecorator
{
	GENERATED_BODY()
	public:






	//=============================================================================================================================================================================================================================================
	//==================================================================================================================FUNCTIONS==================================================================================================================
	//=============================================================================================================================================================================================================================================

public:
	UBTDecorator_HasTarget();


	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
};
