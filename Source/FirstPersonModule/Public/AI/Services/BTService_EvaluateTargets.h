// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EvaluateTargets.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTService_EvaluateTargets : public UBTService
{
	GENERATED_BODY()
private:


	UPROPERTY(EditAnywhere, Category = "Senses")
		class UTargetEvaluationProfile* TargetProfile;


//=============================================================================================================================================================================================
//==========================================================================================FUNCTIONS==========================================================================================
//=============================================================================================================================================================================================


public:
	UBTService_EvaluateTargets();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData);
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;


	
};
