// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EvaluateThreats.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTService_EvaluateThreats : public UBTService
{
	GENERATED_BODY()
public:

	
	UPROPERTY(EditAnywhere, Category = "Threat Profiling")
		class UThreatEvaluationProfile* ThreatProfile;


	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================

	UBTService_EvaluateThreats();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData);
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	 
	
};
