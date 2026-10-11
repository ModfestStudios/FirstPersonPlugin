// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InteractiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "DoorComponent.generated.h"


UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Closed,
	Closing,
	Stopped,
	Opened,
	Opening
};

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	Swing,
	SlideHorizontal,
	SlideVertical,
	DrawBridge
};

USTRUCT()
struct FDoorReplicatedState
{
	GENERATED_BODY()
public:
	
	/*the new state the door is in*/
	UPROPERTY()
		EDoorState NewState = EDoorState::Closed;
	UPROPERTY()
		EDoorState PreviousState = EDoorState::Closed;
	/*the door's current position*/
	UPROPERTY()
		float CurrentPosition = 0.0f;
	/*when the server executed this so we can determine if we want to animate it or just flip its state*/
	UPROPERTY()
		float ServerStartTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FDoorAnimation
{
	GENERATED_BODY()
public:

	/*if enabled - the system will interporlate the 0-1 range of the selected curve for the motion. Otherwise it defaults to a simple linear curve*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		bool bAnimateUsingCurve = false;
	/*door motion*/
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (EditCondition = "bAnimateUsingCurve"))
		UCurveFloat* AnimationCurve;	
	/*how far open/closed the door will be by the end*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		float TargetPosition = 0;
	/*how long it takes to open the door*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		float AnimationDuration = 1.5f;
};

/**
 * 
 */
UCLASS(ClassGroup = (Interactives), HideCategories = ("Sockets", "Navigation", Rendering, Physics, "Component Tick"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UDoorComponent : public UInteractiveComponent
{
	GENERATED_BODY()
public:

	//******************
	//******STATE*******
	//******************
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Door")
		float DoorPosition = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
		EDoorState CurrentState = EDoorState::Closed;
	UPROPERTY(BlueprintReadOnly, Category = "Door")
		EDoorState PreviousState = EDoorState::Closed;
	UPROPERTY(ReplicatedUsing = "OnRep_DoorReplicatedState")
		FDoorReplicatedState DoorReplicatedState;


	//*****************
	//*****LOCKING*****
	//*****************
protected:
	
	UPROPERTY(EditAnywhere, Category = "Door|Locking", meta=(EditCondition="!IsDoorOpen()"))
		bool bLocked = false;
	UPROPERTY(EditAnywhere, Category = "Door|Locking")
		bool bRequiresKey = false;
	UPROPERTY(EditAnywhere, Category = "Door|Locking", meta = (EditCondition="bRequiresKey"))
		TSubclassOf<AActor> KeyClass;



	//********************
	//*****ANIMATIONS*****
	//********************
protected:
	UPROPERTY()
		FTransform InitialDoorTransform;
	/*implementation only supports Swing currently*/
	UPROPERTY(EditAnywhere, Category = "Door|Animations")
		EDoorType DoorType = EDoorType::Swing;
	/*door is closed - and is now being opened*/
	UPROPERTY(EditAnywhere, Category = "Door|Animations")
		FDoorAnimation OpenAnimation;
	/*door is open - and now we're closing it*/
	UPROPERTY(EditAnywhere, Category = "Door|Animations")
		FDoorAnimation CloseAnimation;
	/*door was attempted to be opened - but it was locked. Allowing a small jiggle to the door to visually indicate locked*/
	UPROPERTY(EditAnywhere, Category = "Door|Animations")
		FDoorAnimation LockedJiggleAnimation;
	/*door was violently opened by force (a kick*/
	UPROPERTY(EditAnywhere, Category = "Door|Animations")
		FDoorAnimation KickedOpenAnimation;

	UPROPERTY()
		FDoorAnimation CurrentAnimation;
	float AnimationStartPosition = 0.0f;
	FTimeline DoorTimeline;

	/*if a client receive an animation event that's greater than this value - it will snap instead of animate since it's been too long and they're desynced or just joining*/
	const float MaxAnimationReplicationDelay = 3.5f;


	//===================================================================================================================================================================================
	//=====================================================================================FUNCTIONS=====================================================================================
	//===================================================================================================================================================================================
public:
	UDoorComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void OnRegister() override;
	virtual void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//======================================
	//=============INTERACTIONS=============
	//======================================

public:	
	/*opens the door*/
	UFUNCTION(BlueprintCallable, Category = "Door|Interactions")
		void OpenDoor();
	/*closes the door*/
	UFUNCTION(BlueprintCallable, Category = "Door|Interactions")
		void CloseDoor();
	/*stops the door mid-motion*/
	UFUNCTION(BlueprintCallable, Category = "Door")
		void StopDoor();


	//===============================
	//=============STATE=============
	//===============================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		EDoorState GetCurrentDoorState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		EDoorState GetPreviousDoorState() const;
	/*returns true if door is open, partially open, mid-open or even mid-closing*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorOpen() const;	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorOpening() const;
	UFUNCTION(BlueprintCallable, Category = "Door|Interactions")
		bool  CanDoorOpen() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorClosed() const;	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorClosing() const;
	UFUNCTION(BlueprintCallable, Category = "Door|State")
		bool CanDoorClose() const;

	/*whether or not our door is in an in-between state - and stopped from moving further*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorStopped() const;
	UFUNCTION(BlueprintPure, Category = "Door")
		bool CanDoorStop() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool IsDoorLocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool CanDoorBeLocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door|State")
		bool DoesLockRequireKey() const;





protected:
	UFUNCTION()
		virtual void SetDoorState(EDoorState NewState);
	UFUNCTION()
		virtual void OnDoorStateChanged(EDoorState NewState, EDoorState PrevState);


protected:
	/*called when receiving a new door state rep notify object*/
	UFUNCTION()
		void OnRep_DoorReplicatedState();




	void StartDoorMovement(float TargetPosition);

public:


	UFUNCTION(BlueprintPure, Category = "Door")
		float GetDoorPosition() const { return DoorPosition; }

	//====================================
	//=============ANIMATIONS=============
	//====================================
private:
	/*if an animation is currently active*/
	UFUNCTION()
		bool IsAnimationInProgress();
	/*starts a new door animation*/
	UFUNCTION()
		void PlayDoorAnimation(const FDoorAnimation& DoorAnimation);
	/*called to update the door*/
	UFUNCTION()
		void AnimateDoor(float Value);
	UFUNCTION()
		void OnAnimationFinished();

	/*updates the door's position according to its swing type*/
	UFUNCTION()
		virtual void UpdateDoorTransform();
	/*automatically just snaps the door to the needed position based on a state - used for when a client needs to quickly update its position to sync out-of-date door states*/
	UFUNCTION()
		virtual void SnapDoorToState(EDoorState State);



	class UCurveFloat* GetDefaultLinearCurve();

	UFUNCTION()
		USceneComponent* GetDoorPivot();

	
};
