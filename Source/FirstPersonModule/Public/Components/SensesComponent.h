// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Services/BTService_EvaluateSenses.h"
#include "SensesComponent.generated.h"



struct FActorEvaluation;

UCLASS(ClassGroup = (ArtificialIntelligence), meta = (BlueprintSpawnableComponent), HideCategories = (Activation, "Components|Activation", Collision))
class FIRSTPERSONMODULE_API USensesComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		bool bHasVision = true;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		float MaxVisionDistance = 2400.0f;	
	

protected:
	/*list of all actors we have sensed recently*/
	UPROPERTY()
		TArray<FActorEvaluation> SensedActors;

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


	UFUNCTION(BlueprintCallable, Category = "Senses")
		TArray<FActorEvaluation> GetAllKnownActors();

	UFUNCTION(BlueprintCallable, Category = "Evaluation")
		float GetDistanceToActor(AActor* Actor);

	//==================
	//======VISION======
	//==================

	UFUNCTION(BlueprintCallable, Category = "Evaluation|Visual")
		virtual bool IsWithinVisualRange(AActor* Actor);
};
