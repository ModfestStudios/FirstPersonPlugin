// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

USTRUCT(BlueprintType)
struct FDoorAnimation
{
	GENERATED_BODY()
public:

	/*door motion*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		UCurveFloat* AnimationCurve;
	/*how far open/closed the door will be by the end*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		float TargetDistance = 100;
	/*how long it takes to open the door*/
	UPROPERTY(EditAnywhere, Category = "Animation")
		float AnimationDuration = 1.5f;
};

/**
 * 
 */
UCLASS(ClassGroup = (Interactives), HideCategories = ("Sockets", "Navigation", Rendering, Physics, "Component Tick"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UDoorComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
protected:

	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_OpenedStateChanged", EditAnywhere, Category = "Door")
		bool bOpened = false;
private:
	/*the local state of the door - tracking animation as well as allowing replication to know the state it needs to transition from*/
	UPROPERTY()
		EDoorState DoorState;
protected:
	
	UPROPERTY(EditAnywhere, Category = "Door", meta=(EditCondition="bOpened = false"))
		bool bLocked = false;
	/*implementation only supports Swing currently*/
	UPROPERTY(EditAnywhere, Category = "Door")
		EDoorType DoorType = EDoorType::Swing;
	UPROPERTY(EditAnywhere, Category = "Door")
		bool bReverseDirection = false;
	
	//====================
	//=====ANIMATIONS=====
	//====================
	FTimeline DoorTimeline;
	FDoorAnimation CurrentAnimation;

	/*door is closed - and is now being opened*/
	UPROPERTY(EditAnywhere, Category = "Door Animations")
		FDoorAnimation OpenAnimation;
	/*door is open - and now we're closing it*/
	UPROPERTY(EditAnywhere, Category = "Door Animations")
		FDoorAnimation CloseAnimation;
	/*door was attempted to be opened - but it was locked. Allowing a small jiggle to the door to visually indicate locked*/
	UPROPERTY(EditAnywhere, Category = "Door Animations")
		FDoorAnimation LockedJiggleAnimation;
	/*door was violently opened by force (a kick*/
	UPROPERTY(EditAnywhere, Category = "Door Animations")
		FDoorAnimation KickedOpenAnimation;

	
	
	




	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

	UDoorComponent(const FObjectInitializer& ObjectInitializer);
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
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		void OpenDoor();
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		void CloseDoor();
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		bool  CanDoorOpen();
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		bool CanDoorClose();

	UFUNCTION()
		void UpdateDoorPosition();

protected:
	UFUNCTION()
		void OnRep_OpenedStateChanged();

	//====================================
	//=============ANIMATIONS=============
	//====================================
private:
	UFUNCTION()
		bool IsAnimationInProgress();
	UFUNCTION()
		void PlayDoorAnimation(const FDoorAnimation& DoorAnimation, bool bReverse = false);
	UFUNCTION()
		void AnimateDoor(float Value);
	UFUNCTION()
		void OnAnimationFinished();


	class UCurveFloat* GetDefaultCurve();

	UFUNCTION()
		USceneComponent* GetDoorPivot();

	
};
