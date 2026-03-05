// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIBehaviorSubsystem.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "AI Behavior Subsystem")
class FIRSTPERSONMODULE_API UAIBehaviorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:


	/*lists*/
	UPROPERTY()
		TArray<class AFirstPersonAIController*> AIControllers;
	UPROPERTY()
		TArray<class USensesComponent*> AISensesComponents;


	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================








public:
	UFUNCTION()
		virtual void RegisterAIController(class AFirstPersonAIController* Controller);
	UFUNCTION()
		virtual void RegisterSensesComponent(class USensesComponent* SensesComponent);
	
};
