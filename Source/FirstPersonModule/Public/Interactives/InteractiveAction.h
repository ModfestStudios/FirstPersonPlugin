// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, abstract)
class FIRSTPERSONMODULE_API UInteractiveAction : public UObject
{
	GENERATED_BODY()
public:

    /*the display text for interaction*/
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
        FText InteractionLabel;



	//===========================================================================================
	//=========================================FUNCTIONS=========================================
	//===========================================================================================

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
        void OnInteracationBegin(AActor* OwningActor, AFirstPersonCharacter* User);

    /*user prematurely stopped interaction*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
        void OnInteractionCanceled(AActor* OwningActor, AFirstPersonCharacter* User);

    /*interaction has fully completed*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
        void OnInteractionCompleted(AActor* OwningActor,AFirstPersonCharacter* User);

    /*determines if this Action is available currently*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
        bool CanExecute(AActor* OwningActor, AFirstPersonCharacter* User);
};
