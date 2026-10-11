// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/UnrealType.h"
#include "CollisionShape.h"
#include "Interactives/Interactives.h"
#include "Engine/CollisionProfile.h"
#include "Characters/FirstPersonCharacter.h"
#include "Interactives/InteractiveAction.h"
#include "InteractiveManagerComponent.generated.h"



UENUM()
enum class EInteractiveTraceType : uint8
{
	Controller,
	Camera
};

UENUM()
enum class EInteractionStatus : uint8
{
	Unknown,
	Completed,
	Canceled,
	Failed
};

USTRUCT()
struct FInteraction
{
	GENERATED_BODY()
public:

	UPROPERTY()
		UActorComponent* Interactive = nullptr;
	UPROPERTY()
		EInteractionStatus Status;


	FInteraction() {};

};



UCLASS(BlueprintType, ClassGroup = (Interactives), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UInteractiveManagerComponent : public UActorComponent
{
	friend class UInteractiveComponent;

	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		TEnumAsByte<ETraceTypeQuery> TraceChannel;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		EInteractiveTraceType TraceType;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		float TraceLength;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		float ProbeSize = 2.0f;



	UPROPERTY(EditDefaultsOnly, Category = "Interactions|Debug")
		bool bDebugTrace = false;

	class UCameraComponent* PlayerCamera;


	//******************************
	//*********INTERACTIONS*********
	//******************************

private:
	/*all possible interactions near us*/
	UPROPERTY()
		TArray<class UInteractiveComponent*> NearbyInteractives;
	UPROPERTY()
		TArray<class UInteractiveComponent*> TracedInteractives;
	UPROPERTY()
		class UInteractiveComponent* FocusedInteractive;



	/*the current interaction we're actively using (may be cleared instantly if the interaction was fire and forget)*/
	UPROPERTY()
		class UInteractiveComponent* ActiveInteractive;
	/*the current ACTION we're performing (may be cleared instantly if the interaction was fire and forget)*/
	UPROPERTY()
		class UInteractiveAction* ActiveAction = nullptr;
	/*the world timestamp given when we started performing an action*/
	float InteractionStartTime = 0.0f;

	/*index-key for which current interaction in the stack we're looking at*/
	UPROPERTY()
		int32 PendingInteractiveIndex = 0;
	/*useful reference on the action Action itself we're looking at*/
	UPROPERTY()
		class UInteractiveAction* PendingAction = nullptr;


	/*events/delegates*/
public:
	/*called whenever this Interactive Manager detects we're looking at an interactive item or move away from looking at it*/
	UPROPERTY(BlueprintAssignable)
		FOnInteractiveHoverChange OnInteractiveHoverStateChanged;
	UPROPERTY(BlueprintAssignable)
		FOnInteraction OnInteractionDenied;



	//=============================================================================================================================================================================================
	//==========================================================================================FUNCTIONS==========================================================================================
	//=============================================================================================================================================================================================

public:
	// Sets default values for this component's properties
	UInteractiveManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//================================================
	//==================INTERACTIONS==================
	//================================================
public:
	/*returns the interactive component we're currently focused on with our view*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactions|Interactives")
		virtual class UInteractiveComponent* GetFocusedInteractive();

	UFUNCTION(BlueprintCallable, Category = "Interactions|Interactives")
		virtual void StartInteraction(class UInteractiveComponent* Interactive, class UInteractiveAction* Action);
	UFUNCTION()
		virtual void StopInteraction(class UInteractiveComponent* Interactive, class UInteractiveAction* Action, bool bWasInterrupted = false);

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_StartInteraction(class UInteractiveComponent* Interactive, const FGuid& ID);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Server_StopInteraction(class UInteractiveComponent* Interactive, const FGuid& ID);


protected:
	/*callbacks for the manager - called by the component itself */
	UFUNCTION()
		virtual void NativeOnInteractionStarted(class UInteractiveComponent* Interactive, const class UInteractiveAction* Action);
	UFUNCTION()
		virtual void NativeOnInteractionCompleted(class UInteractiveComponent* Interactive, const class UInteractiveAction* Action);
	UFUNCTION()
		virtual void NativeOnInteractionCancelled(class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, bool bInterrupted);
	UFUNCTION()
		virtual void NativeOnInteractionDenied(class UInteractiveComponent* Interactive, class UInteractiveAction* Action);


	//=====================================
	//===============ACTIONS===============
	//=====================================
public:
	/*gets a list of all actions our current interactive contains (will show disabled/non-usable actions if bIncludeUnusables is checked)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactions|Actions")
		TArray<UInteractiveAction*> GetFocusedInteractiveActions() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactions|Actions")
		UInteractiveAction* GetPendingInteractiveAction() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactions|Actions")
		TArray<FText> GetInteractiveActions();
	UFUNCTION(BlueprintCallable, Category = "Interactions|Actions")
		virtual void NextPendingAction();
	UFUNCTION(BlueprintCallable, Category = "Interactions|Actions")
		virtual void PreviousPendingAction();
	/*simple helper function that sets the Pending Action reference based on the Index (refreshing its pointer)*/
	UFUNCTION(BlueprintCallable, Category = "Interactions|Actions")
		virtual void UpdatePendingActionReference();



	//============================================
	//===========INTERACTIVES MANAGEMENT===========
	//============================================
protected:

	/*checks our traced interactives - and determines which one should be the main focus*/
	UFUNCTION()
		virtual void CalcFocusedInteractive();
	UFUNCTION()
		virtual void CalcInteractives();
	
	UFUNCTION()
		virtual bool IsInteractive(class UInteractiveComponent* Interactive);
	UFUNCTION()
		virtual void AddInteractive(class UInteractiveComponent*& Interactive);
	UFUNCTION()
		virtual void RemoveInteractive(class UInteractiveComponent*& Interactive);
	UFUNCTION()
		virtual void ClearInteractives();
	UFUNCTION()
		virtual void ClearActiveInteraction();
	UFUNCTION()
		virtual void UpdateActiveInteractiveStatus();
	UFUNCTION()
		virtual void NotifyInteractiveOfHover(class UInteractiveComponent*& Interactive);
	UFUNCTION()
		virtual void NotifyInteractiveOfUnhover(class UInteractiveComponent*& Interactive);
public:
	UFUNCTION(BlueprintPure, Category = "Interactives")
		UInteractiveComponent* GetCurrentInteractive() const;
	UFUNCTION(BlueprintPure, Category = "Interactives")
		AActor* GetCurrentInteractiveOwner();
	UFUNCTION(BlueprintPure, Category = "Interactives")
		virtual bool IsActiveInteractionComplete();
	/*returns true if we're currently trying to interact with something*/
	UFUNCTION(BlueprintPure, Category = "Interactives")
		virtual bool IsInteracting();


	//===============================
	//============TRACING============
	//===============================
protected:
	UFUNCTION()
		virtual void TraceForInteractives();
	UFUNCTION()
		virtual FVector GetTraceStart();
	UFUNCTION()
		virtual FVector GetTraceEnd();
	UFUNCTION()
		virtual FRotator GetTraceRotation();
	UFUNCTION()
		virtual ECollisionChannel GetTraceChannel();

	
	//===============================
	//===========UTILITIES===========
	//===============================

	UFUNCTION()
		class UInteractiveComponent* GetInteractiveComponentFromHit(const FHitResult& HitResult) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities")
		class AFirstPersonCharacter* GetCharacterOwner() const;
	
	class UCameraComponent* GetOwnerCamera();


};
