// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/InteractiveManagerComponent.h"
#include "Interactives/InteractiveAction.h"
#include "Interfaces/InteractiveShapeInterface.h"
#include "InteractiveCollisionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionAction, class AFirstPersonCharacter*, User, const class UInteractiveAction*, Action);

USTRUCT(BlueprintType)
struct FInteractiveActionCall
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		TSubclassOf<UInteractiveAction> Action;
	/*if filled - will automatically call the blueprint or C++ with matching function name on the owning actor when activated. Note: Use FInteractionEventParams as a parameter of the function to get references to Action and InteractiveCollisionComponent passed in. MUST BE A UFUNCTION() for C++ to work*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		FName FunctionToCall;
};

USTRUCT(BlueprintType)
struct FInteractionEventParams
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		const UInteractiveAction* Action;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		const class AFirstPersonCharacter* User;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		UInteractiveCollisionComponent* CollisionComponent;

	FInteractionEventParams()
		: Action(nullptr), User(nullptr), CollisionComponent(nullptr) {}
};


UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	//instantly complete the action
	Instant,
	//time-based trigger
	Duration,
	//begins interaction and waits to be canceled/completed externally
	Manual
};

/**
 *
 */
UCLASS(ClassGroup = (Interactions), HideCategories = ("Sockets", "Navigation", Rendering, Physics, "Component Tick"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UInteractiveCollisionComponent : public UBoxComponent
{
	GENERATED_BODY()
public:
	/*whether or not this object can be interacted with currently - attempting to use an interactive that has this set as false will immedietely cancel the action - still shows up in traces*/
	UPROPERTY(Replicated, EditAnywhere, Category = "Interaction")
		bool bIsInteractive = true;
	/*when disabled - will be disregarded by a InteractiveManager's trace entirely - preventing it from showing up in the list*/
	UPROPERTY(Replicated, EditAnywhere, Category = "Interaction")
		bool bInteractionDisabled = false;

	uint8 InteractionIndex = 0; //the current index of our interactions the player is set to


	/*replicated variable indicating this is already being interacted with - used to prevent further use by others*/
	UPROPERTY(Replicated)
		bool bInteractionActive;
	UPROPERTY(Replicated)
		AActor* CurrentUser;
	UPROPERTY(EditAnywhere, Category = "Interaction")
		EInteractionType InteractionType;

	/*list of interactions this item can have*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
		TArray<FInteractiveActionCall> Interactions;	

	/*local variable to be updated by local client only - indicates when this is being hovered*/
	bool bHovered = false;

	/*DELEGATES/EVENTS*/

	///*interaction started*/
	UPROPERTY(BlueprintAssignable)
		FInteractionAction OnInteractionStarted;
	/*called when a user lets go of the interaction*/
	UPROPERTY(BlueprintAssignable)
		FInteractionAction OnInteractionStopped;
	///*succesful completion of interaction reached*/
	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionCompleted;
	///*interaction interrupted before completion*/
	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionCanceled;
	///*callde if someone attempts to interact with this - but cannot*/
	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionDenied;

	UPROPERTY(BlueprintAssignable)
		FOnInteraction OnInteractiveHovered;
	UPROPERTY(BlueprintAssignable)
		FOnInteraction OnInteractiveUnhovered;

	//=================================================================================================
	//============================================FUNCTIONS============================================
	//=================================================================================================

	UInteractiveCollisionComponent();

	//============================================
	//===================PROMPT===================
	//============================================

public:
	/*automatically updates the InteractivePromptComponent with the current action's text*/
	UFUNCTION(BlueprintCallable, Category = "Interaction|Prompt")
		virtual void UpdatePromptText();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction|Prompt")
		const TArray<FText> GetActionLabels(class AFirstPersonCharacter* User);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction|Prompt")
		const FText GetCurrentActionLabel();


	//=========================================
	//=================ACTIONS=================
	//=========================================
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction|Actions")
		const UInteractiveAction* GetCurrentAction() const;
	UFUNCTION(BlueprintCallable, Category = "Interaction|Actions")
		const FName GetCurrentActionFunctionToCall() const;


	//=================================================
	//===================INTERACTION===================
	//=================================================

	/*whether or not the object has any interactions available or potentially busy*/
	UFUNCTION()
		virtual bool IsInteractionAvailable(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);
	/*checks to see if interactions are enabled - hides it from InteractionManager if true*/
	UFUNCTION()
		virtual bool IsInteractionEnabled();
	/*called hwen au ser begins interaction*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual bool BeginInteraction(class AFirstPersonCharacter* User);
	/*function that a player can call directly to try and stop an interaction*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void StopInteraction(class AFirstPersonCharacter* User);



	/*cancel an interaction due to interruption or abandonment*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void CancelInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	/*notify component the interaction has completed*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void CompleteInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	/*interaction attempt was denied*/
	UFUNCTION()
		virtual void DenyInteraction(class AFirstPersonCharacter* User);

public:
	UFUNCTION()
		virtual void OnHoverBegin(class AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void OnHoverEnd(class AFirstPersonCharacter* User);

protected:
	UFUNCTION()
		virtual void TogglePromptVisibility(bool bNewVisibility);

public:
	UFUNCTION()
		virtual float GetCurrentInteractionTime();
	UFUNCTION()
		virtual float GetRemainingInteractionTime();


	//=============================================================
	//==========================UTILITIES==========================
	//=============================================================
	
	/*will find and call a UFUNCTION on the owner that matches the FunctionToCall name*/
	UFUNCTION()
		void CallOwnerFunctionByName(const FName FunctionName,const class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	UFUNCTION()
		virtual void SetActiveUser(AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void RemoveActiveUser(AFirstPersonCharacter* User);

};
