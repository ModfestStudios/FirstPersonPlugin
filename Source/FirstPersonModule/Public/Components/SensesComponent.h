// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Services/BTService_EvaluateSenses.h"
#include "SensesComponent.generated.h"

UENUM(BlueprintType)
enum class EActorAwarenessState : uint8
{
	Unaware,
	Suspicious,
	Lost,
	Tracking,
	Detected,	
};


USTRUCT(BlueprintType)
struct FSensesEvaluation
{	
	GENERATED_BODY()
public:


	/*visual details*/
	UPROPERTY(BlueprintReadOnly, Category = "Vision")
		TObjectPtr<AActor> Actor = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Vision")
		EActorAwarenessState AwarenessState = EActorAwarenessState::Unaware;
	UPROPERTY(BlueprintReadOnly, Category = "Vision")		
		bool bCurrentlySeen = false;
	/*how well the AI can see this actor based on the distance mapped against this AI's VisualFalloff*/
	UPROPERTY()
		float SightStrength;
	UPROPERTY(BlueprintReadOnly)
		float LastSeenTime = -1.0f;
	/*basic counter of how long we've seen this actor in one continous sight (with grace period factored)*/
	UPROPERTY(BlueprintReadOnly)
		float TimeInSight = 0.0f;
	UPROPERTY(BlueprintReadOnly)
		float TimeInSightToDetect = 1.0f;
	/*basic count of how long it's been since we last saw this actor*/
	UPROPERTY(BlueprintReadOnly)
		float TimeSinceSightLost = 0.0f;

	UPROPERTY(BlueprintReadOnly)
		float Distance = -1.0f;
	UPROPERTY(BlueprintReadOnly)
		FVector LastSeenLocation = FVector::ZeroVector;
		
};

UCLASS(ClassGroup = (ArtificialIntelligence), meta = (BlueprintSpawnableComponent), HideCategories = (Activation, "Components|Activation", Collision))
class FIRSTPERSONMODULE_API USensesComponent : public UActorComponent
{
	GENERATED_BODY()
public:


//***************
//****VISION*****
//***************

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
	//	bool bHasVision = true;
	///*the maximum distance the AI can see - anything beyond this cannot be "visually" spotted*/
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
	//	float MaxVisionDistance = 2400.0f;
	///*the minimum amount of time an actor must be within this AI's vision for them to spot them*/
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
	//	FVector2D VisualDetectionDelayRange = FVector2D(0.35f, 0.75f);
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
	//	float TimeBeforeSightReset = 2.0f;
	///*used to help calculate "awareness" of creatures within their range based on the max distance - with 1.0f having "immediate" awareness while lower numbers take longer for the AI to "register" something in their sight*/
	///*if a creature's vision is 100,then at 1.0f on the horizontal scale that'd be 100 - At 0.5f it'd be 50. Vertical scale is the "strength" of the vision. So if you keyed X/Z at (0.5, 1.0f) and (1.0f, 0.25f) then the AI's ability to spot something would "degrade" (take longer) between 50 and 100 units*/
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
	//	FRuntimeFloatCurve VisualFalloff;
	


//**************
//****SENSED****
//**************


protected:
	/*list of all actors we have sensed recently*/
	UPROPERTY()
		TMap<TWeakObjectPtr<AActor>,FSensesEvaluation> SensedActors;



	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================

public:	
	// Sets default values for this component's properties
	USensesComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION()
		virtual void EvaluateSenses(class USensesEvaluationProfile* SensesProfile, float DeltaTime);
protected:
	UFUNCTION()
		virtual void EvaluateVision(class USensesEvaluationProfile* SensesProfile, float DeltaTime);
	/*UFUNCTION()
		virtual void EvaluateHearing();
	UFUNCTION()
		virtual void EvaluateSmell();*/

public:
	UFUNCTION(BlueprintCallable, Category = "Senses")
		int32 GetNumOfActorsSensing();


public:

	UFUNCTION(BlueprintCallable, Category = "Senses")
		FSensesEvaluation GetActorEvaluation(AActor* Actor) const;

	//UFUNCTION(BlueprintCallable, Category = "Senses")
	//	TArray<FActorEvaluation> GetAllKnownActors();

	UFUNCTION(BlueprintCallable, Category = "Evaluation")
		float GetDistanceToActor(AActor* Actor);

	//==================
	//======VISION======
	//==================

	UFUNCTION(BlueprintCallable, Category = "Evaluation|Visual")
		virtual bool IsWithinVisualRange(AActor* Actor, float MaxVisionDistance);
		/*returns true if the actor has been consistently "in-sight" beyond the minimum time required*/
	UFUNCTION(BlueprintCallable, Category = "Evaluation|Visual")
		virtual bool HasActorExceededVisualDetectionTime(AActor* Actor);
	/*returns how well we can see this particular actor based on their distance and our Visual Falloff*/
	UFUNCTION(BlueprintCallable, Category = "Evaluation|Visual")
		virtual float GetVisualStrengthAgainstActor(AActor* Actor, FRuntimeFloatCurve VisualFallofCurve, float MaxVisionDistance);


	//=====================
	//======DETECTION======
	//=====================
	UFUNCTION(BlueprintCallable, Category = "Evaluation|Detection")
		virtual TArray<AActor*> GetDetectedActors() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Evaluation|Detection")
		virtual int32 GetNumberOfDetectedActors() const;



	//=======================
	//=======UTILITIES=======
	//=======================
public:
	const TSet<AActor*>* GetActorsToEvaluate() const;
	//TArray<AActor*> GetActorsToEvaluate();
};
