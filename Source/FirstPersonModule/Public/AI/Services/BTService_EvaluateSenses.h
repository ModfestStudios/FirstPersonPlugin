// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Curves/CurveFloat.h"
#include "BTService_EvaluateSenses.generated.h"

//USTRUCT(BlueprintType)
//struct FActorEvaluation
//{
//	GENERATED_BODY()
//
//
//	UPROPERTY()
//		class AActor* Actor;
//
//};

/** UBTService_EvaluateSenses - Service that simply runs checks and updates the Senses Component of information
 *  such as whether we see someone, heard something, or notice some other sort of sensible (pun) thing
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTService_EvaluateSenses : public UBTService
{
	GENERATED_BODY()
private:
	

	UPROPERTY(EditAnywhere, Category = "Senses")
		class USensesEvaluationProfile* SensesProfile;


//=============================================================================================================================================================================================
//==========================================================================================FUNCTIONS==========================================================================================
//=============================================================================================================================================================================================


public:
	UBTService_EvaluateSenses();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData);
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

	
	
};
