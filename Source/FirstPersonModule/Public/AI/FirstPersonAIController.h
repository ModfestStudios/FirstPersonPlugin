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
	AFirstPersonAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;


	//============================
	//==========ATTACKS===========
	//============================
public:
	virtual void Attack(class UAttackAsset* Attack);



	//===========================
	//==========TARGETS==========
	//===========================
public:
	UFUNCTION(BlueprintCallable, Category = "Targets")
		virtual bool HasTarget();
	UFUNCTION(BlueprintCallable, Category = "Targets")
		virtual void SetTarget(AActor* NewTarget);
	UFUNCTION(BlueprintCallable, Category = "Targets")
		virtual AActor* GetTarget() const;

	//=========================
	//=========THREATS=========
	//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual AActor* GetClosestThreat();
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual AActor* GetHighestThreat();
	UFUNCTION(BlueprintCallable, Category = "Threats")
		virtual AActor* GetLowestThreat();


	//====================
	//========PAWN========	
	//====================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pawn")
		class AFirstPersonCharacter* GetFirstPersonCharacter();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Senses")
		class USensesComponent* GetPawnSensesComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Behavior")
		class UBehaviorComponent* GetPawnBehaviorComponent();


	
};
