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

	/** Unscaled half-size of the interaction collision box. */
	UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (ClampMin = "0.0", UIMin = "0.0"))
		FVector BoxExtent = FVector(25.0f);

	/** Line thickness used to draw the interaction collision box. */
	UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (ClampMin = "0.0", UIMin = "0.0"))
		float CollisionLineThickness = 2.0f;

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Sphere", EditConditionHides))
	//	float SphereRadius = 25.0f;

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Capsule", EditConditionHides))
	//	float CapsuleRadius = 25.0f;

	//UPROPERTY(EditAnywhere, Category = "Interaction|Collision", meta = (EditCondition = "InteractiveShape == EInteractiveShape::Capsule", EditConditionHides))
	//	float CapsuleHalfHeight = 50.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Hover")
		bool bHighlightOnHover = false;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Hover", meta = (EditCondition = "bHighlightOnHover"))
		FName HighlightMaterialParameterName = "Highlight Intensity";
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Hover", meta = (EditCondition = "bHighlightOnHover"))
		float HighlightMaterialIntensity = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Hover", meta = (EditCondition = "bHighlightOnHover"))
		float DefaultMaterialIntensity = 0.0f;

	UPROPERTY()
		UMaterialInstanceDynamic* DynamicMaterial = nullptr;


	///*if enabled - will automatically find the UFUNCTION() (required) in the owner's C++ code and call it*/
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	//	bool bAutoCallFunction = false;
	///*name of the UFUNCTION() to call in C++ of this component's owner*/
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction",meta = (EditCondition=bAutoCallFunction))
	//	FName FunctionToCall;


	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionStarted;
	//UPROPERTY(BlueprintAssignable)
	//	FOnInteraction OnInteractionStopped;


	UPROPERTY(EditAnywhere, Category = "Interaction|Animation")
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
		bool bInteractionActive = false;
    float ActiveInteractionDuration = 0.0f;
	UPROPERTY()
		class UInteractiveAction* ActiveAction = nullptr;


	UPROPERTY(Replicated)
		AFirstPersonCharacter* ActiveUser;

	UPROPERTY(Transient)
		TArray<UInteractiveAction*> RuntimeActions;

	/*list of interactions this item can have*/
	UPROPERTY(EditAnywhere, Category = "Interaction", meta = (DisplayName = "Actions", TitleProperty="Action"))
		TArray<FInteractiveActionConfig> ActionConfigs;

	/*local variable to be updated by local client only - indicates when this is being hovered*/
	bool bHovered = false;


	/**/
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Grid")
		FIntPoint RegisteredGridKey;


	/*DELEGATES/EVENTS*/
public:
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	UFUNCTION()
		virtual void InitializeActions();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;


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
		UInteractiveAction* GetActionByID(FGuid ID) const;
	UFUNCTION(BlueprintCallable, Category = "Interaction|Actions")
		const FName GetCurrentActionFunctionToCall() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction|Actions")
		TArray<UInteractiveAction*> GetAvailableActions(class AFirstPersonCharacter* User) const;
	/*used to determine if an ACTION is available (otherwise it's hidden)*/
	UFUNCTION()
		virtual bool IsActionAvailable(class AFirstPersonCharacter* User, const class UInteractiveAction* Action) const;

	//=================================================
	//===================INTERACTION===================
	//=================================================
public:
	
	
    UFUNCTION(BlueprintPure, Category="Interactions")
        FInteractionContext GetInteractionContext() const;

	/*checks to see if interactions are enabled - hides it from InteractionManager if true*/
	UFUNCTION()
		virtual bool IsInteractionEnabled() const;
	UFUNCTION()
		virtual bool IsActionInProgress(const class UInteractiveAction* Action) const;

	/*function called to initiate an interaction*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual bool NativeOnBeginInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action, const FInteractionContext& Context);
	/*function called when an interaction has been stopped either intentionally or unintentionally*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void NativeOnStopInteraction(class AFirstPersonCharacter* User,const class UInteractiveAction* Action, EInteractionEndReason EndReason, const FInteractionContext& Context);

protected:

	/*cancel an interaction due to interruption or abandonment*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void CancelInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	/*notify component the interaction has completed*/
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void CompleteInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	/*interaction attempt was denied*/
	UFUNCTION()
		virtual void NativeOnDenyInteraction(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);


public:
	UFUNCTION()
		virtual float GetCurrentInteractionTime();
	UFUNCTION()
		virtual float GetRemainingInteractionTime();


	//===============================================================
	//===========================COLLISION===========================
	//===============================================================
protected:
	void UpdateCollisionProperties();

	//==============================================================
	//=========================REGISTRATION=========================
	//==============================================================
protected:
	UFUNCTION()
		virtual void RegisterInteractive();
	UFUNCTION()
		virtual void UnregisterInteractive();

	//==============================================================
	//=============================GRID=============================
	//=============================================================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction|Grid")
		virtual FIntPoint GetGridKey() const;
	UFUNCTION(BlueprintCallable, Category = "Interaction|Grid")
	virtual void UpdateGridKey(FIntPoint NewGridKey);

	//=============================================================
	//==========================UTILITIES==========================
	//=============================================================
protected:
	/*will find and call a UFUNCTION on the owner that matches the FunctionToCall name*/
	UFUNCTION()
		void CallOwnerFunctionByName(const FName FunctionName, const class AFirstPersonCharacter* User, const class UInteractiveAction* Action, const FInteractionContext& Context);

	UFUNCTION()
		virtual void SetActiveUser(AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void RemoveActiveUser(AFirstPersonCharacter* User);
		
};
