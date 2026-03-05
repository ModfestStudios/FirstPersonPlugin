// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FirstPersonAIController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonAIController : public AAIController
{
	GENERATED_BODY()
public:
	

private:
	/*who we're currently trying to unalive*/
	UPROPERTY()
		AActor* CurrentTarget;
	UPROPERTY()
		AActor* PreviousTarget;
	


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================
public:
	AFirstPersonAIController();

	virtual void BeginPlay() override;

	//===========================
	//==========TARGETS==========
	//===========================
public:
	UFUNCTION(BlueprintCallable, Category = "Targets")
		virtual void SetTarget(AActor* NewTarget);

	//=========================
	//=========THREATS=========
	//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual AActor* GetClosestThreat();
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual AActor* GetHighestThreat();

	
};
