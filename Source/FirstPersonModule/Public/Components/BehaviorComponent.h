// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorComponent.generated.h"


UENUM(BlueprintType)
enum class EActorRelationship : uint8
{
	Unknown,
	Friendly,
	Neautral,
	Enemy
};

USTRUCT(BlueprintType)
struct FTargetPriorityEvaluation
{
	GENERATED_BODY()
public:


};

USTRUCT(BlueprintType)
struct FThreatEvaluation
{
	GENERATED_BODY()
public:

	/*
	UPROPERTY(BlueprintReadOnly)
		float Aggression = 0.0f;
	UPROPERTY(BlueprintReadOnly)
		float Weapon = 0.0f;
	UPROPERTY(BlueprintReadOnly)
		float DamageToUs = 0.0f;
	UPROPERTY(BlueprintReadOnly)
		float TargetingUs = 0.0f;*/
	UPROPERTY(BlueprintReadOnly)
		float Distance = 0.0f;


	float GetThreatScore()
	{
		return Distance;
	}
};

USTRUCT(BlueprintType)
struct FActorEvaluation
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		class AActor* Actor = nullptr;
	UPROPERTY(BlueprintReadOnly)
		EActorRelationship Relationship;	
	UPROPERTY(BlueprintReadOnly)
		FThreatEvaluation Threat;
	UPROPERTY(BlueprintReadOnly)
		FTargetPriorityEvaluation TargetPriority;

};




UCLASS( ClassGroup=(ArtificialIntelligences), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Behavior Tree")
		class UBehaviorTree* BehaviorTree;



protected:
	UPROPERTY()
		TMap<TWeakObjectPtr<AActor>, FActorEvaluation> EvaluatedActors;


	//=====================================================================================================================================================================================
	//======================================================================================FUNCTIONS======================================================================================
	//=====================================================================================================================================================================================

public:	
	// Sets default values for this component's properties
	UBehaviorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION()
		virtual void InitBehaviorTree();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//===============================
	//============TARGETS============
	//===============================
public:
	UFUNCTION(BlueprintCallable, Category = "Targets")
		virtual void EvaluateTargets(class UTargetEvaluationProfile* TargetProfile, float DeltaTime);


	//===============================
	//============THREATS============
	//===============================
public:
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual void EvaluateThreats(class UThreatEvaluationProfile* ThreatProfile, float DeltaTime);


	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual float GetCurveScore(const FRuntimeFloatCurve& CurveProfile, const float InDistance);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Threats")
		virtual AActor* GetHighestThreat();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Threats")
		virtual AActor* GetLowestThreat();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Threats")
		virtual AActor* GetClosestThreat();



	//=================================
	//============UTILITIES============
	//=================================
public:
	UFUNCTION()
		class USensesComponent* GetOwningPawnSensesComponent() const;


		
};
