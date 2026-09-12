// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactives/Interactives.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InteractiveManagerComponent.h"
#include "InteractiveComponent.generated.h"


UCLASS(ClassGroup = ("Interactives"), meta = (BlueprintSpawnableComponent), HideCategories = ("Physics", "Component Tick", "Lighting", "Cooking", "Mesh Painting"))
class FIRSTPERSONMODULE_API UInteractiveComponent : public USceneComponent
{
	GENERATED_BODY()
public:

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision")
	//	EInteractiveShape InteractiveShape = EInteractiveShape::Box;

	UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Box", EditConditionHides))
		FVector BoxExtent = FVector(25.0f);

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Sphere", EditConditionHides))
	//	float SphereRadius = 25.0f;

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Capsule", EditConditionHides))
	//	float CapsuleRadius = 25.0f;

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Capsule", EditConditionHides))
	//	float CapsuleHalfHeight = 50.0f;



	UPROPERTY(EditDefaultsOnly, Category = "Material")
		bool bHighlightOnHover = false;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FName HighlightMaterialParameterName = "Highlight Intensity";
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float HighlightMaterialIntensity = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float DefaultMaterialIntensity = 0.0f;

	UPROPERTY()
		UMaterialInstanceDynamic* DynamicMaterial = nullptr;



	/*if enabled - will automatically find the UFUNCTION() (required) in the owner's C++ code and call it*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
		bool bAutoCallFunction = false;
	/*name of the UFUNCTION() to call in C++ of this component's owner*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction",meta = (EditCondition=bAutoCallFunction))
		FName FunctionToCall;


	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionStarted;
	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionStopped;



	UPROPERTY(EditAnywhere, Category = "Animation")
		UCurveFloat* AnimationCurve;

	UPROPERTY(Transient)
		class UBoxComponent* InteractiveCollision = nullptr;	

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
		FOnInteraction OnInteractionBegin;
	/*called when a user lets go of the interaction*/
	UPROPERTY(BlueprintAssignable)
		FOnInteractionEnded OnInteractionEnd;

	UPROPERTY(BlueprintAssignable)
		FInteractionHover OnInteractiveHovered;
	UPROPERTY(BlueprintAssignable)
		FInteractionHover OnInteractiveUnhovered;

	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================

public:	
	// Sets default values for this component's properties
	UInteractiveComponent();
	UFUNCTION()
		virtual void OnRegister() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



protected:
	///*will find and call a UFUNCTION on the owner that matches the FunctionToCall name*/
	//UFUNCTION()
	//	void CallOwnerFunctionByName();


	//============================
	//============HOVER===========
	//============================
protected:
	UFUNCTION(BlueprintCallable, Category = "Interaction|Hover")
		virtual void InitDynamicMaterial();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction|Hover")
		virtual UStaticMeshComponent* GetHoverStaticMesh();
	UFUNCTION(BlueprintCallable, Category = "Interaction|Hover")
		virtual void HighlightDynamicMaterial();
	UFUNCTION(BlueprintCallable, Category = "Interaction|Hover")
		virtual void RemoveHighlightFromDynamicMaterial();

public:
	UFUNCTION()
		virtual void NativeOnHovered(class AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void NativeOnUnhovered(class AFirstPersonCharacter* User);	

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

protected:
	UFUNCTION()
		virtual void TogglePromptVisibility(bool bNewVisibility);


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
public:
	/*whether or not the object has any interactions available or potentially busy*/
	UFUNCTION()
		virtual bool IsInteractionAvailable(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);
	/*checks to see if interactions are enabled - hides it from InteractionManager if true*/
	UFUNCTION()
		virtual bool IsInteractionEnabled();

	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual bool BeginInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

public:
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
		virtual float GetCurrentInteractionTime();
	UFUNCTION()
		virtual float GetRemainingInteractionTime();


	//===============================================================
	//===========================COLLISION===========================
	//===============================================================
//protected:
//	UFUNCTION()
//		virtual void RebuildCollision();
//	UFUNCTION()
//		virtual void UpdateCollisionProperties();

		


	//=============================================================
	//==========================UTILITIES==========================
	//=============================================================
protected:
	/*will find and call a UFUNCTION on the owner that matches the FunctionToCall name*/
	UFUNCTION()
		void CallOwnerFunctionByName(const FName FunctionName, const class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	UFUNCTION()
		virtual void SetActiveUser(AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void RemoveActiveUser(AFirstPersonCharacter* User);
		
};
