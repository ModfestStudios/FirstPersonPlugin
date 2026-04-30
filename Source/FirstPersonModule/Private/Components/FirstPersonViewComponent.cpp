// Copyright (c) 2022 Pocket Sized Animations


#include "Components/FirstPersonViewComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animations/FirstPersonArmsAnimInstance.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*curves*/
#include "Curves/CurveFloat.h"

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
	/*all clients run setup*/
	InitializeFirstPersonScene();
	InitializeCameraComponent();
	InitializeArmsMesh();
	SyncPawnEyeHeight();
	DefaultFOV = GetCameraComponent()->FieldOfView;
	InitializeDamageIndictators();


	Super::InitializeComponent();

}


// Called when the game starts
void UFirstPersonViewComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UFirstPersonViewComponent::InitializeFirstPersonScene()
{
	if (Scene != nullptr || GetOwner() == nullptr)
		return;

	/*create Root Scene*/
	Scene = NewObject<USceneComponent>(GetOwner(), FName("FirstPersonScene"), RF_Transient);
	if (Scene)
	{
		/*initialize*/
		Scene->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Scene->RegisterComponent();
		Scene->Activate();

		/*position*/
		Scene->SetRelativeLocation(FVector(ViewForwardOffset, 0, ViewHeight));
	}
}

void UFirstPersonViewComponent::InitializeCameraComponent()
{
	/*safety checks*/
	if (Camera != nullptr || Scene == nullptr || GetOwner() == nullptr)
		return;

	/*create camera*/
	Camera = NewObject<UCameraComponent>(GetOwner(), FName("FirstPersonCamera"), RF_Transient);
	if (Camera)
	{
		/*initialize*/
		Camera->AttachToComponent(Scene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Camera->RegisterComponent();
		Camera->Activate();
	}
}

void UFirstPersonViewComponent::InitializeArmsMesh()
{
	if (Arms != nullptr || ArmsMesh == nullptr)
		return;

	/*create arms*/
	Arms = NewObject<USkeletalMeshComponent>(GetOwner(), ArmsMeshName, RF_Transient);
	if (Arms)
	{
		/*initialize*/
		Arms->AttachToComponent(Scene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Arms->RegisterComponent();
		Arms->Activate();

		/*setup placement*/
		Arms->SetRelativeLocation(ArmsOffset);

		/*initialize visuals*/
		Arms->SetSkeletalMesh(ArmsMesh);
		Arms->SetAnimInstanceClass(AnimationBlueprint);
		Arms->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);

		/*disable shadow casting*/
		Arms->SetCastShadow(false);

		/*ensure we hide this from anyone but the owning player*/
		Arms->SetOnlyOwnerSee(true);
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

void UFirstPersonViewComponent::SyncPawnEyeHeight()
{
	if (APawn* P = Cast<APawn>(GetOwner()))
	{
		P->BaseEyeHeight = ViewHeight;
	}
}

// Called every frame
void UFirstPersonViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*stop updates while camera is snapped to camera*/
	if (bSnapCameraToHead)
		return;

	if (GetOwningCharacter()->IsLocallyControlled())
	{
		/*up/down looking*/
		FRotator newSceneRotation = Scene->GetRelativeRotation();
		newSceneRotation.Pitch = Pitch;

		Scene->SetRelativeRotation(newSceneRotation);

		/*determines when arms should snap to camera's vertical pitch and when they should snap to scene*/
		if (bAutoCalcVerticalFreeLook)
		{
			if (ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::LockedInPlace)
				SetFreeLookMode(EViewType::LockedInPlace);
			else if (!ShouldArmsLockToCamera() && GetFreeLookMode() != EViewType::VerticalFreeLook)
				SetFreeLookMode(EViewType::VerticalFreeLook);
		}

		/*camera free look updates*/
		if (bAllowFreeLook)
		{
			if (bSnapSceneToCameraOnReset)
			{
				FRotator newRotation = Camera->GetComponentRotation();

				Scene->SetWorldRotation(newRotation);
				Camera->SetRelativeRotation(FRotator(0, 0, 0));

				/*reset*/
				bSnapSceneToCameraOnReset = false;
			}
			else
			{
				FRotator newCameraRotation = Camera->GetRelativeRotation();
				newCameraRotation.Pitch = VerticalFreeLook; //add camera offset
				newCameraRotation.Yaw = HorizontalFreeLook; //add camera offset

				Camera->SetRelativeRotation(newCameraRotation);
			}
		}

		/*zooming in/out updates*/
		if (bAllowZoom && GetOwner()->GetWorldTimerManager().IsTimerActive(ZoomAnimHandler))
		{
			AnimateVisionZoom();
		}
	}
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
		UE_LOG(LogTemp, Log, TEXT("CamSocket: %s"), *CamSocket.ToString());


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
		Value *= -1;

	if (ViewType >= EViewType::VerticalFreeLook)
	{
		VerticalFreeLook += Value;
		//todo: Apply offsetting against Scene pitch so you can't go past 89 between the total of Scene + Camera pitching
		VerticalFreeLook = FMath::Clamp(VerticalFreeLook, -89, 89);
	}
	else
	{
		Pitch += Value;
		Pitch = FMath::Clamp(Pitch, -89, 89);
	}
}

void UFirstPersonViewComponent::RotateView(float Value)
{
	if (Value == 0.0f)
		return;

	if (ViewType == EViewType::FullFreeLook)
	{
		HorizontalFreeLook += Value;
		HorizontalFreeLook = FMath::Clamp(HorizontalFreeLook, -89, 89);
	}
	/*rotate controller*/
	else
	{
		if (APawn* Pawn = Cast<APawn>(GetOwner()))
			Pawn->AddControllerYawInput(Value);
	}
}

void UFirstPersonViewComponent::ZoomInVision()
{
	/*some safety nets to keep us from activating this while it's already active*/
	if (bZoomingIn)
		return;

	/*begin zoom*/
	bZoomingIn = true;
	GetOwner()->GetWorldTimerManager().SetTimer(ZoomAnimHandler, this, &UFirstPersonViewComponent::AnimateVisionZoom, ZoomInDuration, true);
}

void UFirstPersonViewComponent::ZoomOutVision()
{
	if (!bZoomingIn)
		return;

	/*begin zoom*/
	bZoomingIn = false;
	GetOwner()->GetWorldTimerManager().SetTimer(ZoomAnimHandler, this, &UFirstPersonViewComponent::AnimateVisionZoom, ZoomOutDuration, true);
}

void UFirstPersonViewComponent::AnimateVisionZoom()
{
	if (bZoomingIn)
	{
		float ElapsedTime = GetOwner()->GetWorldTimerManager().GetTimerElapsed(ZoomAnimHandler);
		//float TimeRemaining = ZoomInDuration - ElapsedTime; //remaining time
		float Math = (ElapsedTime / ZoomInDuration);
		CurrentZoomAmount = 1.0 + (ZoomAmount * Math); //Get the % of completed time and multiply that against the total desired zoom
		UE_LOG(LogTemp, Log, TEXT("UFirstPersonViewComponent::AnimateVisionZoom() - IN : Elapsed (%f) Zoomy Math (%f) CurrentZoomAmount (%f)"), ElapsedTime, Math, CurrentZoomAmount);

		if (CurrentZoomAmount >= ZoomAmount)
			GetOwner()->GetWorldTimerManager().ClearTimer(ZoomAnimHandler);
	}
	else
	{
		float ElapsedTime = GetOwner()->GetWorldTimerManager().GetTimerElapsed(ZoomAnimHandler);
		float Math = ElapsedTime / ZoomOutDuration; //remaining time
		CurrentZoomAmount = CurrentZoomAmount - Math;

		UE_LOG(LogTemp, Log, TEXT("UFirstPersonViewComponent::AnimateVisionZoom() - OUT : Elapsed (%f) Zoomy Math (%f) CurrentZoomAmount (%f)"), ElapsedTime, Math, CurrentZoomAmount);
		if (CurrentZoomAmount <= 1.0f)
			GetOwner()->GetWorldTimerManager().ClearTimer(ZoomAnimHandler);
	}

	CurrentZoomAmount = FMath::Clamp(CurrentZoomAmount, 1.0f, ZoomAmount); //clamp for safety!
	float FOVDifference = DefaultFOV - (DefaultFOV * CurrentZoomAmount);
	CurrentFOV = DefaultFOV - FMath::Abs(FOVDifference);

	GetCameraComponent()->SetFieldOfView(CurrentFOV);
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

void UFirstPersonViewComponent::SetFirstPersonArms(USkeletalMesh* NewMesh)
{
	ArmsMesh = NewMesh;
}

void UFirstPersonViewComponent::SetFirstPersonAnimBlueprint(TSubclassOf<class UFirstPersonArmsAnimInstance> NewAnimInstance)
{
	AnimationBlueprint = NewAnimInstance;

	Arms->SetAnimInstanceClass(AnimationBlueprint);
	Arms->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);
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
