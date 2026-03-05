// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Curves/CurveFloat.h"
#include "BTService_EvaluateSenses.generated.h"

USTRUCT(BlueprintType)
struct FActorEvaluation
{
	GENERATED_BODY()


	UPROPERTY()
		class AActor* Actor;

};

/** UBTService_EvaluateSenses - Service that simply runs checks and updates the Senses Component of information
 *  such as whether we see someone, heard something, or notice some other sort of sensible (pun) thing
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTService_EvaluateSenses : public UBTService
{
	GENERATED_BODY()
private:
	
	UPROPERTY(EditAnywhere, Category = "Vision")
		FRuntimeFloatCurve VisualRangeAwareness;

	/*how often (in seconds) we should check to see if any new actors should be considered for evaluation*/
	UPROPERTY(EditAnywhere, Category = "Evaluation")
		float CacheUpdateRate = 30.0f;

	/*cached component*/
	class USensesComponent* SensesComponent;
	/*a list of all actors in the world that we will evaluate to see if this character "senses" them to any degree*/
	UPROPERTY()
		TArray<FActorEvaluation> ActorsToEvaluate;
	/*timestamp of when we last grabbed the latest list of Actors to evaluate*/
	UPROPERTY()
		float LastCacheUpdate = -27.0f;	//we set it to -27 initially to trigger the initial update quickly


public:
	UBTService_EvaluateSenses();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData);
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	class USensesComponent* GetOwnerSensesComponent(class AAIController* AIController);
	float GetDistanceFromActor(UBehaviorTreeComponent& OwnerComp, AActor* Target);
	TArray<AActor*> GetActorsToEvaluate();
	bool ShouldUpdateEvaluationList();
	void UpdateEvaluationList();
	bool IsEvaluatingActor(AActor* Actor);
	
	
};
