// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractivePromptInfo.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UInteractivePromptInfo : public UUserWidget
{
	GENERATED_BODY()
public:



protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Prompt")
		FText ActionLabel = NSLOCTEXT("InteractiveActions", "GenericInteract", "Interact");



	//=====================================================================================================================
	//======================================================FUNCTIONS======================================================
	//=====================================================================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Prompt")
		virtual void SetActionLabel(FText NewLabel);
};
