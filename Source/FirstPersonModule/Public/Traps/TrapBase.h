// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapBase.generated.h"

UCLASS(HideCategories = (ActorTick,ComponentTick,Clothing,LeaderPoseComponent,Physics,AnimationRig,Lighting,Deformer,SkinWeights,Rendering,HLOD,Navigation,VirtualTexture,Tags,Replication,ComponentReplication,Activation,Cooking,Actor,Input))
class FIRSTPERSONMODULE_API ATrap : public AActor
{
	GENERATED_BODY()
public:


protected:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TrapActivated)
		bool bTrapActivated = false;
	
	/*probably a useful property - mostly meant for traps that have a retracted state*/
	UPROPERTY(Replicated)
		bool bRetracted = false;

	UPROPERTY(EditAnywhere, Category = " Trap")
		float DamageAmount = 1000.0f;
	/*when enabled - will allow damage to happen even when trap is inactive (for sharp or pointy things you run into)*/
	UPROPERTY(EditAnywhere, Category = " Trap")
		bool bDamageWhenInactive = false;	
	UPROPERTY(EditAnywhere, Category = " Trap", meta = (EditoCondition = "bDamageWhenInactive"))
		float InactiveDamageAmount = 10.0f;
	UPROPERTY(EditAnywhere, Category = " Trap")
		TSubclassOf<UDamageType> DamageType;
	/*if true - will apply damage every X using DamageRate*/
	UPROPERTY(EditAnywhere, Category = " Trap")
		bool bUseDamageRate = false;
	UPROPERTY(EditAnywhere, Category = " Trap")
		float DamageRate = 0.35f;
	FTimerHandle DamageRateHandler;
	

	/*used to track actors we should apply damage to when configured to be a rate-based-damage-thingy*/
	TArray<AActor*> ActorsToDamage;
	
	/*when turned on - trap will be activatically activate as needed*/
	UPROPERTY(EditAnywhere, Category = "Trap")
		bool bAutoActivate = false;
	/*if set - will automatically reset after required time has passed. Otherwise needs to be called manually*/
	UPROPERTY(EditAnywhere, Category = "Trap")
		bool bAutoReset = true;
	
	/*how long this trap should stay active for*/
	UPROPERTY(EditAnywhere,Category = "Trap")
		float ActivationTime = 2.0f;
	
	/*true activation time*/
	UPROPERTY()
		float InternalActivationTime = 2.0f;
	/*random +/- to the ActivationTime*/
	UPROPERTY(EditAnywhere, Category = "Trap")
		float ActivationTimeVariance = 0.2f;
	
	/*timestamp of when this trap was last activated*/
	float LastActiveTimestamp = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Trap|Knockback")
		float KnockbackStrength = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Trap|Knockback")
		float KnockbackUpwardStrength = 200.0f;






	//=================================================================================================================
	//====================================================FUNCTIONS====================================================
	//=================================================================================================================

	
public:	
	// Sets default values for this actor's properties
	ATrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;




public:
	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void ActivateTrap();
	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void ResetTrap();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trap")
		bool IsTrapActivated();

	UFUNCTION()
		void OnRep_TrapActivated();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trap")
		bool IsTrapRetracted();
	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void RetractTrap();
	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void ProtractTrap();



	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void DamageActor(AActor* Target);
	

	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void AddActorToDamage(AActor* Target);
	UFUNCTION(BlueprintCallable, Category = "Trap")
		virtual void RemoveActorToDamage(AActor* Target);
	/*called from a timer to damage all actors marked for damage*/
	UFUNCTION()
		virtual void DamageAllActors();

	/*fun little function that just tosses a character away from the center of the trap*/
	UFUNCTION()
		virtual void PushCharacterBack(ACharacter* Character,const float BackwardStrength = 1200.0f,const float UpwardStrength = 200.0f);


	//============UTILITIES============
	/*returns how long this trap has been active for*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trap")
		float GetActiveElapsedTime();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trap")
		bool HasElapsedTimeExceededDesiredDuration();

};
