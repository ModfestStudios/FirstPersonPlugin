// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperatingSystem/OperatingSystem.h"
#include "Blueprint/UserWidget.h"
#include "OperatingSystemWidget.generated.h"



/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UOperatingSystemWidget : public UUserWidget
{
	GENERATED_BODY()
public:



protected:
	/*the operating system this widget is rendering*/
	UPROPERTY(BlueprintReadOnly, Category = "Operating System")
		class AOperatingSystem* OperatingSystem;	



	//=============================================================================================================================================
	//==================================================================FUNCTIONS==================================================================
	//=============================================================================================================================================
public:
	UOperatingSystemWidget(const FObjectInitializer& ObjectInitializer);

	/*function used to initialize the widget based on the Operating System passed in*/
	UFUNCTION(BlueprintCallable, Category = "Operating System")
		virtual void InitializeWidget(class AOperatingSystem* OS);

	/*returns the world-placed terminal that's hosting the operating system (if exists at all)*/
	UFUNCTION(BlueprintPure, Category = "Operating System")
		class ATerminalSystem* GetTerminalSystem();
	UFUNCTION(BlueprintPure, Category = "Operating System")
		EOperatingSystemMainState GetOperatingSystemState();



	
	
};
