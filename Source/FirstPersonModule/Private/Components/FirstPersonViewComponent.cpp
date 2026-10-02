// Copyright (c) 2022 Pocket Sized Animations


#include "Components/FirstPersonViewComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animations/FirstPersonArmsAnimInstance.h"

/*camera*/
#include "EngineCamerasSubsystem.h"
#include "CameraAnimationSequence.h"
#include "Animations/CameraAnimationCameraModifier.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*curves*/
#include "Curves/CurveFloat.h"

/*components*/
#include "Components/FirstPersonMovementComponent.h"

/*engine*/
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"

/*materials*/
#include "Materials/MaterialInstanceDynamic.h"

/*players*/
#include "Players/FirstPersonPlayerController.h"

/*utilities*/
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"


/*ui*/
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/PlayerHUDWidget.h"

// Sets default values for this component's properties
UFirstPersonViewComponent::UFirstPersonViewComponent(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultArmMeshRef(TEXT("/FirstPersonModule/Characters/Mesh/MaleMannequin_Quin_FirstPersonArms"));
	if (DefaultArmMeshRef.Succeeded())
		ArmsMesh = DefaultArmMeshRef.Object;	
	ArmsOffset = FVector(0, 0, -15);

	//ArmsAnimationClass = UFirstPersonArmsAnimInstance::StaticClass();
	bWantsInitializeComponent = true;

	bAutoActivate = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "Custom Detail"
	static const FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);

	//Change "Actor" for the type of your Class(eg. Actor, Pawn, CharacterMovementComponent)
	//Change "MySection" to the name of Desired Section
	TSharedRef<FPropertySection> Section = PropertyModule.FindOrCreateSection("FirstPersonViewComponent", "Free Look", LOCTEXT("Free Look", "Free Look"));

	//You can add multiples categories to be tracked by this section
	Section->AddCategory("Free Look");
#undef LOCTEXT_NAMESPACE
#endif
}

#if WITH_EDITOR
void UFirstPersonViewComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	/*FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UFirstPersonViewComponent, ViewHeight))
	{
		SyncPawnEyeHeight();
	}*/

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UFirstPersonViewComponent::PostInitProperties()
{
	Super::PostInitProperties();
	SyncPawnEyeHeight();
}

void UFirstPersonViewComponent::InitializeComponent()
{
	Super::InitializeComponent();

	/*all clients run setup*/
	InitializeFirstPersonViewRoot();
	InitializeFirstPersonBodyRoot();

	InitializeCameraComponent();

	InitializeArmsMesh();
	InitializeBodyMesh();
	InitializeLegsMesh();

	SyncPawnEyeHeight();
	DefaultFOV = GetCameraComponent()->FieldOfView;
	InitializeDamageIndictators();
}


// Called when the game starts
void UFirstPersonViewComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AFirstPersonCharacter* Character = GetOwningCharacter())
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			AddTickPrerequisiteComponent(Movement);
		}
	}

	/*used to make sure things are properly hidden/shown during startup*/
	UpdateFirstPersonVisibility();

#if WITH_EDITOR
	/*bind switching between Play In Editor (PIE) and Simulate In Editor (SIE)*/
	FEditorDelegates::OnSwitchBeginPIEAndSIE.AddUObject(this, &UFirstPersonViewComponent::OnSwitchPIEAndSIE);
#endif
}

void UFirstPersonViewComponent::InitializeFirstPersonViewRoot()
{
	if (FirstPersonViewRoot != nullptr || GetOwner() == nullptr)
		return;

	/*create Root Scene*/
	FirstPersonViewRoot = NewObject<USceneComponent>(GetOwner(), FName("FirstPersonViewRoot"), RF_Transient);
	if (FirstPersonViewRoot)
	{
		/*initialize*/
		FirstPersonViewRoot->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FirstPersonViewRoot->RegisterComponent();
		FirstPersonViewRoot->Activate();

		/*position*/
		FirstPersonViewRoot->SetRelativeLocation(FVector(ViewForwardOffset, 0, StandingViewHeight));
	}
}

void UFirstPersonViewComponent::InitializeFirstPersonBodyRoot()
{
	if (FirstPersonBodyRoot != nullptr || GetOwner() == nullptr)
		return;

	/*create Root Scene*/
	FirstPersonBodyRoot = NewObject<USceneComponent>(GetOwner(), FName("FirstPersonBodyRoot"), RF_Transient);
	if (FirstPersonBodyRoot)
	{
		/*initialize*/
		FirstPersonBodyRoot->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FirstPersonBodyRoot->RegisterComponent();
		FirstPersonBodyRoot->Activate();
	}
}

void UFirstPersonViewComponent::InitializeCameraComponent()
{
	/*safety checks*/
	if (Camera != nullptr || FirstPersonViewRoot == nullptr || GetOwner() == nullptr)
		return;

	/*create camera*/
	Camera = NewObject<UCameraComponent>(GetOwner(), FName("FirstPersonCamera"), RF_Transient);
	if (Camera)
	{
		/*initialize*/
		Camera->AttachToComponent(FirstPersonViewRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Camera->RegisterComponent();
		Camera->Activate();
	}

	DefaultFOV = GetCameraComponent()->FieldOfView;
	CurrentFOV = DefaultFOV;
	CurrentZoomAmount = 1.0f;
	ZoomStartAmount = 1.0f;
}

void UFirstPersonViewComponent::InitializeArmsMesh()
{
	if (ArmsMeshComponent != nullptr || ArmsMesh == nullptr)
		return;

	/*create arms*/
	ArmsMeshComponent = NewObject<USkeletalMeshComponent>(GetOwner(), ArmsMeshName, RF_Transient);
	if (ArmsMeshComponent)
	{
		/*initialize*/
		ArmsMeshComponent->AttachToComponent(FirstPersonViewRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ArmsMeshComponent->RegisterComponent();
		ArmsMeshComponent->Activate();

		/*setup placement*/
		ArmsMeshComponent->SetRelativeLocation(ArmsOffset);

		/*initialize visuals*/
		ArmsMeshComponent->SetSkeletalMesh(ArmsMesh);
		ArmsMeshComponent->SetAnimInstanceClass(ArmsAnimationBlueprint);
		ArmsMeshComponent->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);

		/*disable shadow casting*/
		ArmsMeshComponent->SetCastShadow(false);

		/*ensure we hide this from anyone but the owning player*/
		ArmsMeshComponent->SetOnlyOwnerSee(true);
		ArmsMeshComponent->SetVisibility(false,false);
		ArmsMeshComponent->SetHiddenInGame(true,true);

		/*disable collision*/
		ArmsMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ArmsMeshComponent->SetGenerateOverlapEvents(false);
	}
}

void UFirstPersonViewComponent::InitializeBodyMesh()
{
	if(BodyMeshComponent != nullptr || BodyMesh == nullptr)
		return;

	/*create body mesh*/
	BodyMeshComponent = NewObject<USkeletalMeshComponent>(GetOwner(), BodyMeshName, RF_Transient);
	if (BodyMeshComponent)
	{
		/*initialize*/
		BodyMeshComponent->AttachToComponent(FirstPersonBodyRoot,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		BodyMeshComponent->RegisterComponent();
		BodyMeshComponent->Activate();

		/*setup placement*/
		BodyMeshComponent->SetRelativeLocation(BodyOffset);

		/*initialize visuals*/
		BodyMeshComponent->SetSkeletalMesh(BodyMesh);
		BodyMeshComponent->SetAnimInstanceClass(BodyAnimationBlueprint);
		BodyMeshComponent->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);

		/*disable shadows*/
		BodyMeshComponent->SetCastShadow(false);

		/*ensure we hide this from anyone but owning player*/
		BodyMeshComponent->SetOnlyOwnerSee(true);
		BodyMeshComponent->SetVisibility(false,false);
		BodyMeshComponent->SetHiddenInGame(true, true);

		/*disable collision*/
		BodyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BodyMeshComponent->SetGenerateOverlapEvents(false);
	}
}

void UFirstPersonViewComponent::InitializeLegsMesh()
{
	if (LegsMeshComponent != nullptr || LegsMesh == nullptr)
		return;

	/*create body mesh*/
	LegsMeshComponent = NewObject<USkeletalMeshComponent>(GetOwner(), LegsMeshName, RF_Transient);
	if (LegsMeshComponent)
	{
		/*initialize*/
		LegsMeshComponent->AttachToComponent(FirstPersonBodyRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		LegsMeshComponent->RegisterComponent();
		LegsMeshComponent->Activate();

		/*setup placement*/
		LegsMeshComponent->SetRelativeLocation(LegsOffset);

		/*initialize visuals*/
		LegsMeshComponent->SetSkeletalMesh(LegsMesh);
		LegsMeshComponent->SetAnimInstanceClass(LegsAnimationBlueprint);
		LegsMeshComponent->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);

		/*disable shadows*/
		LegsMeshComponent->SetCastShadow(false);

		/*ensure we hide this from anyone but owning player*/
		LegsMeshComponent->SetOnlyOwnerSee(true);
		LegsMeshComponent->SetVisibility(false, false);
		LegsMeshComponent->SetHiddenInGame(true, true);

		/*disable collision*/
		LegsMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LegsMeshComponent->SetGenerateOverlapEvents(false);
	}
}

/* InitializeDamageIndicators() - Used to pre-load the Post Process Materials for smooth experience
* 
*
*
*/

void UFirstPersonViewComponent::InitializeDamageIndictators()
{
	for (FDamageIndicator& Indicator : DamageIndicators)
	{
		/*safety check*/
		if (!Indicator.Material)
			continue;

		Indicator.DynamicMaterial = UMaterialInstanceDynamic::Create(Indicator.Material, this);
		
		if (!Indicator.DynamicMaterial) //skip if failed
			continue;

		Indicator.DynamicMaterial->SetScalarParameterValue("Intensity", 0.0f); //mark invisible at start
		Camera->AddOrUpdateBlendable(Indicator.DynamicMaterial, 0.0f);
	}
}

#if WITH_EDITOR
void UFirstPersonViewComponent::OnSwitchPIEAndSIE(bool bIsSimulating)
{
	if(bIsSimulating)
	{
		SetArmsVisibility(false); // hide arms
		SetBodyVisibility(false);
		SetLegsVisibility(false);
		HidePlayerHUD();
	}
	else
	{
		UpdateFirstPersonVisibility(); //restore first person arms
		ShowPlayerHUD();
	}
}
#endif

void UFirstPersonViewComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
#if WITH_EDITOR
	FEditorDelegates::OnSwitchBeginPIEAndSIE.RemoveAll(this);
#endif

	if (ArmsMeshComponent)
	{
		ArmsMeshComponent->DestroyComponent();
		ArmsMeshComponent = nullptr;
	}

	if (BodyMeshComponent)
	{
		BodyMeshComponent->DestroyComponent();
		BodyMeshComponent = nullptr;
	}

	if (LegsMeshComponent)
	{
		LegsMeshComponent->DestroyComponent();
		LegsMeshComponent = nullptr;
	}

	if (Camera)
	{
		Camera->DestroyComponent();
		Camera = nullptr;
	}

	if (FirstPersonViewRoot)
	{
		FirstPersonViewRoot->DestroyComponent();
		FirstPersonViewRoot = nullptr;
	}

	if (FirstPersonBodyRoot)
	{
		FirstPersonBodyRoot->DestroyComponent();
		FirstPersonBodyRoot = nullptr;
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFirstPersonViewComponent::SyncPawnEyeHeight()
{
	if (APawn* P = Cast<APawn>(GetOwner()))
	{
		P->BaseEyeHeight = StandingViewHeight;
	}
}

void UFirstPersonViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*safety*/
	AFirstPersonCharacter* Character = GetOwningCharacter();

	if (!Character || !FirstPersonViewRoot || !Camera)
		return;

	/*first-person view is only processed for the locally controlled character*/
	if (!Character->IsLocallyControlled())
		return;

	/*camera has temporarily been attached directly to the character's head*/
	if (bSnapCameraToHead)
		return;


	//========================================================================================================================
	//======================================================FREE LOOK=========================================================
	//========================================================================================================================

	if (bAutoCalcVerticalFreeLook && !IsFreeLooking())
	{
		if (ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::LockedInPlace)
		{
			SetFreeLookMode(EViewType::LockedInPlace);
		}
		else if (!ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::VerticalFreeLook)
		{
			SetFreeLookMode(EViewType::VerticalFreeLook);
		}
	}


	/*
	 * When leaving free-look, transfer the Camera's existing local pitch
	 * back into the ViewRoot.
	 */
	if (bSnapSceneToCameraOnReset)
	{
		const float RootPitch = FirstPersonViewRoot->GetRelativeRotation().Pitch;
		const float CameraPitch = Camera->GetRelativeRotation().Pitch;

		Pitch = FMath::Clamp(
			FRotator::NormalizeAxis(RootPitch + CameraPitch),
			-89.0f,
			89.0f
		);

		Camera->SetRelativeRotation(FRotator::ZeroRotator);

		VerticalFreeLook = 0.0f;
		HorizontalFreeLook = 0.0f;

		bSnapSceneToCameraOnReset = false;
	}


	//========================================================================================================================
	//======================================================MANUAL LEAN=======================================================
	//========================================================================================================================

	float DesiredLeanAlpha = 0.0f;

	switch (Character->GetLeanState())
	{
	case ELeanState::Left:
		DesiredLeanAlpha = -1.0f;
		break;

	case ELeanState::Right:
		DesiredLeanAlpha = 1.0f;
		break;

	case ELeanState::None:
	default:
		DesiredLeanAlpha = 0.0f;
		break;
	}


	/*target changed - begin transition from our current position*/
	if (!FMath::IsNearlyEqual(DesiredLeanAlpha, LeanTargetAlpha))
	{
		LeanStartAlpha = CurrentLeanAlpha;
		LeanTargetAlpha = DesiredLeanAlpha;
		LeanElapsedTime = 0.0f;
	}


	if (!FMath::IsNearlyEqual(CurrentLeanAlpha, LeanTargetAlpha))
	{
		const bool bLeaningOut = FMath::IsNearlyZero(LeanTargetAlpha);

		const float BaseDuration =
			bLeaningOut
			? LeanOutDuration
			: LeanInDuration;


		/*
		 * Scale duration based on how much distance remains.
		 *
		 * 0 -> 1   = full duration
		 * .5 -> 0  = half duration
		 * 1 -> -1  = twice duration
		 */
		const float DistanceRemaining =
			FMath::Abs(LeanTargetAlpha - LeanStartAlpha);

		const float TransitionDuration =
			BaseDuration * DistanceRemaining;


		LeanElapsedTime += DeltaTime;


		const float TimeAlpha =
			TransitionDuration > KINDA_SMALL_NUMBER
			? FMath::Clamp(LeanElapsedTime / TransitionDuration, 0.0f, 1.0f)
			: 1.0f;


		/*use curve if supplied*/
		const UCurveFloat* ActiveCurve =
			bLeaningOut
			? LeanOutCurve
			: LeanInCurve;


		/*fallback to linear interpolation*/
		const float CurveAlpha =
			ActiveCurve
			? FMath::Clamp(ActiveCurve->GetFloatValue(TimeAlpha), 0.0f, 1.0f)
			: TimeAlpha;


		CurrentLeanAlpha = FMath::Lerp(
			LeanStartAlpha,
			LeanTargetAlpha,
			CurveAlpha
		);


		/*finish exactly on target*/
		if (TimeAlpha >= 1.0f)
		{
			CurrentLeanAlpha = LeanTargetAlpha;
			LeanElapsedTime = 0.0f;
		}
	}
	else
	{
		CurrentLeanAlpha = LeanTargetAlpha;
	}


	//========================================================================================================================
	//======================================================STRAFE LEAN=======================================================
	//========================================================================================================================

	float TargetStrafeLeanAlpha = 0.0f;


	/*
	 * GetStrafeDirection():
	 *
	 * -1 = left
	 *  0 = not purely strafing
	 * +1 = right
	 *
	 * Intermediate values are supported for analog movement.
	 */
	if (Character->IsStrafing())
	{
		TargetStrafeLeanAlpha = Character->GetStrafeDirection();
	}


	/*
	 * Manual leaning progressively removes the strafe lean.
	 *
	 * Manual Lean = 0.0 -> 100% strafe lean
	 * Manual Lean = 0.5 ->  50% strafe lean
	 * Manual Lean = 1.0 ->   0% strafe lean
	 */
	const float StrafeLeanWeight =
		1.0f - FMath::Abs(CurrentLeanAlpha);

	TargetStrafeLeanAlpha *= StrafeLeanWeight;


	/*
	 * Determine whether we're moving toward center or away from center.
	 *
	 * Moving toward center uses StrafeLeanOutDuration.
	 * Moving away from center uses StrafeLeanInDuration.
	 *
	 * Opposite directions initially count as moving out so that we return
	 * through center using OutDuration, then begin leaning into the opposite
	 * side using InDuration.
	 */
	const bool bChangingStrafeDirection =
		(CurrentStrafeLeanAlpha * TargetStrafeLeanAlpha) < 0.0f;

	const bool bReducingStrafeLean =
		FMath::Abs(TargetStrafeLeanAlpha) < FMath::Abs(CurrentStrafeLeanAlpha);

	const bool bReturningToCenter =
		FMath::IsNearlyZero(TargetStrafeLeanAlpha);

	const bool bStrafeLeaningOut =
		bReturningToCenter ||
		bChangingStrafeDirection ||
		bReducingStrafeLean;


	const float StrafeDuration =
		bStrafeLeaningOut
		? StrafeLeanOutDuration
		: StrafeLeanInDuration;


	/*
	 * Duration represents the amount of time required to travel one full
	 * alpha unit:
	 *
	 * 0 -> 1  = StrafeLeanInDuration
	 * 1 -> 0  = StrafeLeanOutDuration
	 *
	 * Therefore a direct +1 -> -1 transition naturally takes two alpha units.
	 */
	if (StrafeDuration > KINDA_SMALL_NUMBER)
	{
		const float StrafeLeanRate =
			1.0f / StrafeDuration;

		CurrentStrafeLeanAlpha = FMath::FInterpConstantTo(
			CurrentStrafeLeanAlpha,
			TargetStrafeLeanAlpha,
			DeltaTime,
			StrafeLeanRate
		);
	}
	else
	{
		/*zero duration means snap immediately*/
		CurrentStrafeLeanAlpha = TargetStrafeLeanAlpha;
	}


	/*remove tiny residual values*/
	if (FMath::IsNearlyZero(CurrentStrafeLeanAlpha, 0.001f))
	{
		CurrentStrafeLeanAlpha = 0.0f;
	}


	//========================================================================================================================
	//======================================================VIEW HEIGHT=======================================================
	//========================================================================================================================

	const float DesiredViewHeight = GetDesiredViewHeight();

	const float CurrentViewHeight =
		FirstPersonViewRoot->GetRelativeLocation().Z;

	const float NewViewHeight =
		FMath::FInterpTo(
			CurrentViewHeight,
			DesiredViewHeight,
			DeltaTime,
			GetViewHeightInterpSpeed()
		);


	//========================================================================================================================
	//===================================================COMPOSE VIEW ROOT====================================================
	//========================================================================================================================

	/*manual lean controls horizontal position + roll*/
	const float ManualLeanOffset =
		LeanDistance * CurrentLeanAlpha;

	const float ManualLeanRotation =
		LeanRoll * CurrentLeanAlpha;


	/*strafe lean only contributes roll*/
	const float StrafeLeanRotation =
		StrafeLean * CurrentStrafeLeanAlpha;


	/*combine independent roll contributions*/
	const float FinalViewRoll =
		ManualLeanRotation +
		StrafeLeanRotation;


	/*
	 * Final shared ViewRoot transform:
	 *
	 * X = forward offset
	 * Y = manual lean position
	 * Z = stance height
	 *
	 * Pitch = normal view pitch
	 * Yaw   = character/controller
	 * Roll  = manual lean + strafe lean
	 */
	const FVector ViewRootLocation(
		ViewForwardOffset,
		ManualLeanOffset,
		NewViewHeight
	);

	const FRotator ViewRootRotation(
		Pitch,
		0.0f,
		FinalViewRoll
	);


	FirstPersonViewRoot->SetRelativeLocationAndRotation(
		ViewRootLocation,
		ViewRootRotation
	);


	//========================================================================================================================
	//======================================================CAMERA LOOK=======================================================
	//========================================================================================================================

	/*
	 * Free-look remains Camera-local so the Arms inherit the ViewRoot
	 * movement without following Camera-only free-look.
	 */
	FRotator CameraRotation = FRotator::ZeroRotator;

	if (bAllowFreeLook)
	{
		switch (ViewType)
		{
		case EViewType::VerticalFreeLook:

			CameraRotation.Pitch = VerticalFreeLook;
			break;


		case EViewType::FullFreeLook:

			CameraRotation.Pitch = VerticalFreeLook;
			CameraRotation.Yaw = HorizontalFreeLook;
			break;


		case EViewType::LockedInPlace:
		default:

			break;
		}
	}


	Camera->SetRelativeRotation(CameraRotation);
}

void UFirstPersonViewComponent::EnableFreeLook()
{
	SetFreeLookMode(EViewType::FullFreeLook);
}

void UFirstPersonViewComponent::DisableFreeLook()
{
	if (bAutoCalcVerticalFreeLook)
	{
		if (ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::LockedInPlace)
		{
			SetFreeLookMode(EViewType::LockedInPlace);
		}
		else if (!ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::VerticalFreeLook)
		{
			SetFreeLookMode(EViewType::VerticalFreeLook);
		}
	}
	else
		SetFreeLookMode(EViewType::LockedInPlace);

	HorizontalFreeLook = 0.0f;
}

bool UFirstPersonViewComponent::IsFreeLooking() const
{
	return ViewType == EViewType::FullFreeLook;
}

void UFirstPersonViewComponent::SetFreeLookMode(EViewType NewMode)
{
	EViewType previousMode = ViewType;
	ViewType = NewMode;

	if (NewMode == EViewType::LockedInPlace)
	{
		//snap back
		Pitch = VerticalFreeLook;
		VerticalFreeLook = 0.0f;
		HorizontalFreeLook = 0.0f;

		if (previousMode == EViewType::VerticalFreeLook)
			bSnapSceneToCameraOnReset = true;
	}
}

void UFirstPersonViewComponent::AttachCameraToHead(bool bKeepRelativeDistance)
{
	if (GetOwningCharacter())
	{
		bSnapCameraToHead = true;

		FName CamSocket = GetOwningCharacter()->HeadCameraSocket;
		//UE_LOG(LogTemp, Log, TEXT("CamSocket: %s"), *CamSocket.ToString());


		if (CamSocket.IsNone())
			UE_LOG(LogTemp, Warning, TEXT("UFirstPersonViewComponent::AttachCameraToheader() - !!! No Head Socket Found !!!"));

		USkeletalMeshComponent* Mesh = GetOwningCharacter()->GetMesh();
				
		//Camera->SetRelativeLocation(FVector(0, -50, 100));	
				
		if(bKeepRelativeDistance)
			Camera->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, CamSocket);		
		else
			Camera->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CamSocket);
	}
}

void UFirstPersonViewComponent::PitchView(float Value)
{
	if (Value == 0.0f)
		return;

	if (bInvertVerticalLook)
		Value *= -1.0f;

	if (ViewType >= EViewType::VerticalFreeLook)
	{
		VerticalFreeLook += Value;

		VerticalFreeLook = FMath::Clamp(VerticalFreeLook,-VerticalFreeLookLimit,VerticalFreeLookLimit);
	}
	else
	{
		Pitch += Value;
		Pitch = FMath::Clamp(Pitch, -89.0f, 89.0f);
	}
}

void UFirstPersonViewComponent::RotateView(float Value)
{
	if (Value == 0.0f)
		return;

	if (ViewType == EViewType::FullFreeLook)
	{
		HorizontalFreeLook += Value;

		HorizontalFreeLook = FMath::Clamp(HorizontalFreeLook,-HorizontalFreeLookLimit,HorizontalFreeLookLimit);
	}
	else
	{
		if (APawn* Pawn = Cast<APawn>(GetOwner()))
		{
			Pawn->AddControllerYawInput(Value);
		}
	}
}

float UFirstPersonViewComponent::GetDesiredViewHeight() const
{
	if (OwningCharacter)
	{
		if (OwningCharacter->IsProne())
			return ProneViewHeight;
		if(OwningCharacter->IsCrouched())
			return CrouchedViewHeight;		
		else
			return StandingViewHeight;
	}

	return 88.0f;
}

float UFirstPersonViewComponent::GetViewHeightInterpSpeed() const
{
	return ViewHeightInterpSpeed;
}

void UFirstPersonViewComponent::PlayCameraAnimation(UCameraAnimationSequence* AnimationSequence)
{
	if(!AnimationSequence || !GetOwningCharacter())
		return;

	APlayerController* PC = GetOwningCharacter()->GetController<APlayerController>();

	if (UEngineCamerasSubsystem* CSS = UEngineCamerasSubsystem::GetEngineCamerasSubsystem(GetWorld()))
	{
		FCameraAnimationParams CameraParams;
		CameraParams.Scale = 1.0;
		CameraParams.PlayRate = 1.0f;
		CameraParams.bLoop = false;
		CameraParams.PlaySpace = ECameraAnimationPlaySpace::CameraLocal;

		CSS->PlayCameraAnimation(PC,AnimationSequence,CameraParams);
	}
}

void UFirstPersonViewComponent::PlayJumpAnimation()
{
	if(GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled() && JumpAnimation)
		PlayCameraAnimation(JumpAnimation);
}

void UFirstPersonViewComponent::PlayLandedCameraAnimation()
{
	if(GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled() && SoftLandAnimation)
		PlayCameraAnimation(SoftLandAnimation);
}

void UFirstPersonViewComponent::PlayFallDamageCameraAnimation()
{
	if(GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled() && FallDamageAnimation)
		PlayCameraAnimation(FallDamageAnimation);
}


void UFirstPersonViewComponent::ZoomInVision()
{
	if (bZoomingIn)
		return;

	bZoomingIn = true;
	ZoomElapsedTime = 0.0f;
	ZoomStartAmount = CurrentZoomAmount;

	GetOwner()->GetWorldTimerManager().SetTimer(ZoomAnimHandler,this,&UFirstPersonViewComponent::AnimateVisionZoom,0.015f,true);
}

void UFirstPersonViewComponent::ZoomOutVision()
{
if (!bZoomingIn)
		return;

	bZoomingIn = false;
	ZoomElapsedTime = 0.0f;
	ZoomStartAmount = CurrentZoomAmount;

	GetOwner()->GetWorldTimerManager().SetTimer(ZoomAnimHandler,this,&UFirstPersonViewComponent::AnimateVisionZoom,0.015f,true);
}

void UFirstPersonViewComponent::AnimateVisionZoom()
{
	ZoomElapsedTime += GetWorld()->GetDeltaSeconds();

	const float Duration = bZoomingIn ? ZoomInDuration : ZoomOutDuration;

	float Alpha = ZoomElapsedTime / Duration;
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	if (bZoomingIn)
		CurrentZoomAmount = FMath::Lerp(ZoomStartAmount, ZoomAmount, Alpha);
	else
		CurrentZoomAmount = FMath::Lerp(ZoomStartAmount, 1.0f, Alpha);

	float FOVDifference = DefaultFOV - (DefaultFOV * CurrentZoomAmount);
	CurrentFOV = DefaultFOV - FMath::Abs(FOVDifference);

	//UE_LOG(LogTemp, Warning, TEXT("ZOOM | Direction: %s | Elapsed: %.3f | Duration: %.3f | Alpha: %.3f | StartAmount: %.3f | ZoomAmount: %.3f | CurrentAmount: %.3f | DefaultFOV: %.2f | CurrentFOV: %.2f"),
	//	bZoomingIn ? TEXT("IN") : TEXT("OUT"),
	//	ZoomElapsedTime,
	//	Duration,
	//	Alpha,
	//	ZoomStartAmount,
	//	ZoomAmount,
	//	CurrentZoomAmount,
	//	DefaultFOV,
	//	CurrentFOV);

	GetCameraComponent()->SetFieldOfView(CurrentFOV);

	if (Alpha >= 1.0f)
		GetOwner()->GetWorldTimerManager().ClearTimer(ZoomAnimHandler);
	
}

void UFirstPersonViewComponent::ProcessRecenterView()
{
	if (VerticalFreeLook < 0.0f)
	{
		VerticalFreeLook += FreeLookReturnRate;
		VerticalFreeLook = FMath::Clamp(VerticalFreeLook, -89, 0);
	}
	else
	{
		VerticalFreeLook -= FreeLookReturnRate;
		VerticalFreeLook = FMath::Clamp(VerticalFreeLook, 0, 89);
	}

	if (FMath::IsNearlyEqual(VerticalFreeLook, 0, 0.5f))
	{
		VerticalFreeLook = 0.0f;
		GetOwner()->GetWorldTimerManager().ClearTimer(FreeLookReturnHandler);
	}

	Camera->SetRelativeRotation(FRotator(VerticalFreeLook, 0, 0));

}

//======================================
//=============FIRST PERSON=============
//======================================

void UFirstPersonViewComponent::UpdateFirstPersonVisibility()
{
	/*init*/
	APawn* Pawn = Cast<APawn>(GetOwner());
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;

	/*unless we're locally controlled we do not want to see the first person meshes*/
	bool bVisible = Pawn && Pawn->IsLocallyControlled() && PC && PC->IsLocalPlayerController();

	//UE_LOG(
	//	LogTemp,
	//	Warning,
	//	TEXT("%s | PlayerController: %s | Local: %d | Arms Visible: %d"),
	//	*GetOwner()->GetName(),		
	//	PC ? *PC->GetName() : TEXT("NONE"),
	//	Pawn ? Pawn->IsLocallyControlled() : false,
	//	bVisible
	//);

	/*update meshes*/
	SetArmsVisibility(bVisible);
	SetBodyVisibility(bVisible);
	SetLegsVisibility(bVisible);
}

//=====================================
//==========FIRST PERSON ARMS==========
//=====================================

void UFirstPersonViewComponent::SetFirstPersonArms(USkeletalMesh* NewMesh)
{
	ArmsMesh = NewMesh;
}

void UFirstPersonViewComponent::SetFirstPersonAnimBlueprint(TSubclassOf<class UFirstPersonArmsAnimInstance> NewAnimInstance)
{
	ArmsAnimationBlueprint = NewAnimInstance;

	ArmsMeshComponent->SetAnimInstanceClass(ArmsAnimationBlueprint);
	ArmsMeshComponent->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);
}

void UFirstPersonViewComponent::PlayFullArmAnimation(UAnimSequence* Animation)
{
	if(!Animation || !GetArmsMeshComponent() || !GetArmsMeshComponent()->GetAnimInstance() && (GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled()))
		return;

	UAnimInstance* ArmsAnimInstance = GetArmsMeshComponent()->GetAnimInstance();

	ArmsAnimInstance->PlaySlotAnimationAsDynamicMontage(Animation,"Arms");
}

void UFirstPersonViewComponent::PlayLeftArmAnimation(UAnimSequence* Animation)
{
	if (!Animation || !GetArmsMeshComponent() || !GetArmsMeshComponent()->GetAnimInstance() && (GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled()))
		return;

	UAnimInstance* ArmsAnimInstance = GetArmsMeshComponent()->GetAnimInstance();

	ArmsAnimInstance->PlaySlotAnimationAsDynamicMontage(Animation, "LeftArm");
}

void UFirstPersonViewComponent::PlayRightArmAnimation(UAnimSequence* Animation)
{
	if (!Animation || !GetArmsMeshComponent() || !GetArmsMeshComponent()->GetAnimInstance() && (GetOwningCharacter() && GetOwningCharacter()->IsLocallyControlled()))
		return;

	UAnimInstance* ArmsAnimInstance = GetArmsMeshComponent()->GetAnimInstance();

	ArmsAnimInstance->PlaySlotAnimationAsDynamicMontage(Animation, "RightArm");
}


bool UFirstPersonViewComponent::ShouldArmsLockToCamera()
{
	if (GetOwningCharacter())
	{
		if (GetOwningCharacter()->HasWeaponEquipped())
			return true;
	}

	/*fallback value*/
	return false;
}



void UFirstPersonViewComponent::SetArmsVisibility(bool bVisible)
{
	if (ArmsMeshComponent)
	{
		ArmsMeshComponent->SetVisibility(bVisible, true);
		ArmsMeshComponent->SetHiddenInGame(!bVisible, true);
	}	
}

void UFirstPersonViewComponent::SetBodyVisibility(bool bVisible)
{
	if (BodyMeshComponent)
	{
		BodyMeshComponent->SetVisibility(bVisible, true);
		BodyMeshComponent->SetHiddenInGame(!bVisible, true);
	}	
}

void UFirstPersonViewComponent::SetLegsVisibility(bool bVisible)
{
	if (LegsMeshComponent)
	{
		LegsMeshComponent->SetVisibility(bVisible, true);
		LegsMeshComponent->SetHiddenInGame(!bVisible, true);
	}	
}

void UFirstPersonViewComponent::InitializePlayerHUD(APlayerController* PlayerController)
{
	if (!HUDClass || !GetOwner())
		return;

	/*local-pawn check - no need to add widget otherwise*/
	AFirstPersonCharacter* Pawn = Cast<AFirstPersonCharacter>(GetOwner());
	if (!Pawn->IsLocallyControlled())
		return;

	if (PlayerController)
	{
		PlayerHUD = CreateWidget<UPlayerHUDWidget>(PlayerController, HUDClass, "PlayerHUDWidget");
		if (IsValid(PlayerHUD))
		{
			PlayerHUD->InitializeHUD(Cast<APlayerController>(Pawn->GetController()), Pawn);
			PlayerHUD->bMinimalHud = bMinimalHud;
			PlayerHUD->AddToViewport();

			if (bAutoShowHUD)
			{
				PlayerHUD->AddToViewport(999);
			}
		}
	}
}

void UFirstPersonViewComponent::ShowPlayerHUD()
{
	if (!GetOwner() || !PlayerHUD)
		return;

	/*local-pawn check - no need to add widget otherwise*/
	if (AFirstPersonCharacter* Pawn = Cast<AFirstPersonCharacter>(GetOwner()))
	{
		if (!Pawn->IsLocallyControlled())
			return;

		if (AFirstPersonPlayerController* PC = Pawn->GetController<AFirstPersonPlayerController>())
		{
			PlayerHUD->AddToViewport();
		}
	}
}

void UFirstPersonViewComponent::HidePlayerHUD()
{
	if (!GetOwner() || !PlayerHUD)
		return;

	/*local-pawn check - no need to add widget otherwise*/
	if (AFirstPersonCharacter* Pawn = Cast<AFirstPersonCharacter>(GetOwner()))
	{
		if (!Pawn->IsLocallyControlled())
			return;
		if (AFirstPersonPlayerController* PC = Pawn->GetController<AFirstPersonPlayerController>())
		{
			PlayerHUD->RemoveFromViewport();
		}
	}
}


UUserWidget* UFirstPersonViewComponent::GetPlayerHUD()
{
	return PlayerHUD;
}

void UFirstPersonViewComponent::GetCameraView(FMinimalViewInfo& ViewInfo)
{
	Camera->GetCameraView(UGameplayStatics::GetWorldDeltaSeconds(this), ViewInfo);
}

void UFirstPersonViewComponent::SetMinimalHUD(bool bUseMinimalHUD)
{
	if (PlayerHUD)
		PlayerHUD->bMinimalHud = bUseMinimalHUD;
}

//==========================================
//===========DAMAGE INDICATORS==============
//==========================================
void UFirstPersonViewComponent::FlashDamageIndicator(TSubclassOf<UDamageType> DamageType)
{
	/*loop through damage types and add them to be played*/
	for (FDamageIndicator DmgIndicator : DamageIndicators)
	{
		if (DmgIndicator.bFilterByDamageType == false || DmgIndicator.HasMatchingDamageType(DamageType))
		{
			UMaterialInstanceDynamic* MaterialInstance;
			AddDamageIndicatorAnimation(DmgIndicator, MaterialInstance); //add the damage indicator to the system
			Camera->AddOrUpdateBlendable(MaterialInstance, DmgIndicator.BlendWeight); //add the material to our Post Process
		}
	}

	/*check to see if we need to activate our animation handler*/
	if (!IsDamageIndicatorTimerActive())
		ActivateDamageIndicatorTimer(); //play our animations!

}


/*loop through all active animations and check if we already have this animation playing*/
bool UFirstPersonViewComponent::DamageIndicatorActive(FGuid Guid)
{
	for (uint8 i = 0; i < DamageIndicatorAnimations.Num(); i++)
	{
		if (DamageIndicatorAnimations[i].GetID() == Guid)
			return true;
	}

	return false;
}

/*AddDamageInidicatorAnimaiton() - Generates and adds a new Animation Struct to our list of damage indicators*/
void UFirstPersonViewComponent::AddDamageIndicatorAnimation(FDamageIndicator& DamageIndicator, UMaterialInstanceDynamic*& OutMaterialInstance)
{
	FDamageIndicatorAnim NewDmgIndicatorAnim = FDamageIndicatorAnim(this, DamageIndicator.GetID(), DamageIndicator.DynamicMaterial, DamageIndicator.AnimationCurve,DamageIndicator.AnimationLength, GetWorld()->GetTimeSeconds());
	DamageIndicatorAnimations.Add(NewDmgIndicatorAnim); //Add to list of animations
	OutMaterialInstance = NewDmgIndicatorAnim.Material; //assign our newly create material to be sent back out
}

void UFirstPersonViewComponent::RemoveDamageIndicatorAnimation(FDamageIndicatorAnim& DamageIndicatorAnim)
{
	for (uint8 i = 0; i < DamageIndicatorAnimations.Num(); i++)
	{
		if (DamageIndicatorAnimations[i].GetID() == DamageIndicatorAnim.GetID())
		{
			DamageIndicatorAnimations.RemoveAt(i);
			return; //break and return
		}
	}
}


bool UFirstPersonViewComponent::IsDamageIndicatorTimerActive()
{
	return GetOwner()->GetWorldTimerManager().IsTimerActive(DamageAnimHandler);
}

void UFirstPersonViewComponent::ActivateDamageIndicatorTimer()
{
	GetOwner()->GetWorldTimerManager().SetTimer(DamageAnimHandler, this, &UFirstPersonViewComponent::AnimateDamageIndicator, 0.005f, true);
}

void UFirstPersonViewComponent::ClearDamageIndicatorTimer()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(DamageAnimHandler);
}

/*AnimateDamageIndicator() - Loops and plays the "animations" for our Materials*/
void UFirstPersonViewComponent::AnimateDamageIndicator()
{
	if (DamageIndicatorAnimations.Num() == 0)
	{
		ClearDamageIndicatorTimer();
		return;
	}

	/*for each DamageIndicatorAnim currently active - adjust the intensity based on the curve animation*/
	for (int32 i = DamageIndicatorAnimations.Num() -1; i >= 0; --i)
	{
		FDamageIndicatorAnim& DamageAnim = DamageIndicatorAnimations[i];
		if (DamageAnim.Material)
		{
			DamageAnim.UpdateIntensity(this);
		}

		if (DamageAnim.AnimCurve)
		{
			if (DamageAnim.IsComplete(this))
			{
				RemoveDamageIndicatorAnimation(DamageAnim);				
			}
		}
	}
}

void UFirstPersonViewComponent::AddPostProcessMaterial(UMaterialInterface* PostProcessMaterial, float Weight)
{
	Camera->AddOrUpdateBlendable(PostProcessMaterial, Weight);
}

void UFirstPersonViewComponent::RemovePostProcessMaterial(UMaterialInterface* PostProcessMaterial)
{
	Camera->RemoveBlendable(PostProcessMaterial);
}

AFirstPersonCharacter* UFirstPersonViewComponent::GetOwningCharacter()
{
	if (OwningCharacter != nullptr && OwningCharacter == GetOwner())
		return OwningCharacter;
	else
	{
		OwningCharacter = Cast<AFirstPersonCharacter>(GetOwner());
		return OwningCharacter;
	}
}
