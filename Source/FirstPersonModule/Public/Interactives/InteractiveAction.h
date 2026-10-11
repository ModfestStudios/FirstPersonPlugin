// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interactives/Interactives.h"
#include "InteractiveAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, abstract)
class FIRSTPERSONMODULE_API UInteractiveAction : public UObject
{
	GENERATED_BODY()
public:

    UPROPERTY()
        FGuid ID;
    /*the display text for interaction*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
        FText InteractionLabel;


    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        class UInteractiveComponent* OwningInteractive;
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        bool bEnabled;
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        EInteractionType InteractionType;
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        float ActionDuration;
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        EInteractionFunctionType FunctionType;
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        FName FunctionToCall;


	//===========================================================================================
	//=========================================FUNCTIONS=========================================
	//===========================================================================================

public:

    UFUNCTION()
        virtual void Initialize(class UInteractiveComponent* OwningComponent, const FInteractiveActionConfig& Config);


    UFUNCTION()
        virtual void NativeOnInteractionStarted(class UInteractiveComponent* InteractiveComponent, class AActor* InteractiveActor, class AFirstPersonCharacter* User) const;
    UFUNCTION()
        virtual void NativeOnInteractionCompleted(class UInteractiveComponent* InteractiveComponent, class AActor* InteractiveActor, class AFirstPersonCharacter* User) const;
    UFUNCTION()
        virtual void NativeOnInteractionCanceled(class UInteractiveComponent* InteractiveComponent, class AActor* InteractiveActor, class AFirstPersonCharacter* User, bool bInterrupted) const;
    UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta = (DisplayName = "Is Action Available"))
        bool IsActionAvailable(const class UInteractiveComponent* InteractiveComponent, const class AActor* InteractiveActor, const class AFirstPersonCharacter* User) const;




protected:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction", meta = (DisplayName = "On Interaction Started"))
        void BP_OnInteractionStarted(class UInteractiveComponent* InteractiveComponent,class AActor* InteractiveActor,class AFirstPersonCharacter* User) const;
    /*interaction has fully completed*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction", meta = (DisplayName = "On Interaction Completed"))
        void BP_OnInteractionCompleted(class UInteractiveComponent* InteractiveComponent, class AActor* InteractiveActor, class AFirstPersonCharacter* User) const;
    /*user prematurely stopped interaction*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction", meta = (DisplayName = "On Interaction Canceled"))
        void BP_OnInteractionCanceled(class UInteractiveComponent* InteractiveComponent, class AActor* InteractiveActor, class AFirstPersonCharacter* User, bool bInterrupted) const;



};
