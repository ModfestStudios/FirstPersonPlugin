// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasDetectedActor.generated.h"

UENUM(BlueprintType)
enum class EActorDetectionCriteria : uint8
{
	Any,
	LessThan,
	EqualTo,
	GreaterThan
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTDecorator_HasDetectedActor : public UBTDecorator
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Detection Criteria")
		EActorDetectionCriteria DetectionCriteria;
	UPROPERTY(EditAnywhere, Category = "Detection Criteria", meta=(EditCondition="DetectionCriteria != EActorDetectionCriteria::Any", ClampMin="0",UIMin="0"))
		int32 NumberOfActors;




	//=============================================================================================================================================================================================================================================
	//==================================================================================================================FUNCTIONS==================================================================================================================
	//=============================================================================================================================================================================================================================================

	public:
	UBTDecorator_HasDetectedActor();


	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

	
};
