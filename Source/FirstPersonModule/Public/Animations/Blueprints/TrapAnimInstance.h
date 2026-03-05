// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TrapAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UTrapAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Trap")
		bool bTrapActivated = false;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Trap")
		bool bRetracted = false;


	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;




	//==========UTILITIES==========
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "")
		class ATrap* GetOwningTrap();
	
};
