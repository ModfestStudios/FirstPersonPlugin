// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonModule/Public/Characters/FirstPersonCharacter.h"
#include "FirstPersonAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonAICharacter : public AFirstPersonCharacter
{
	GENERATED_BODY()
public:

private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
		class UBehaviorComponent* BehaviorComponent;
	UPROPERTY(VisibleAnywhere, Category = "AI")
		class USensesComponent* SensesComponent;


	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================

	AFirstPersonAICharacter(const FObjectInitializer& ObjectInitializer);
	
};
