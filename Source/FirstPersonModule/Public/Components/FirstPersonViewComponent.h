// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/DamageType.h"
#include "FirstPersonViewComponent.generated.h"


USTRUCT(BlueprintType)
struct FDamageIndicator
{
	GENERATED_BODY()



private:
	/*auto-assigned ID for tracking*/
	UPROPERTY(VisibleAnywhere, Category = "First Person View|General")
		FGuid Guid = FGuid::NewGuid();
public:
	/*the post-process material to apply to our camera*/
	UPROPERTY(EditAnywhere, Category = "First Person View|General")
		class UMaterialInterface* Material;
	/*runtime Material that'll be applied to Post Processing - used for preloading*/
	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicMaterial = nullptr;
	UPROPERTY(EditAnywhere, Category = "First Person View|General")
		bool bFilterByDamageType = false;
	/*the damage types to apply this for*/
	UPROPERTY(EditAnywhere, Category = "First Person View|General", meta = (EditCondition = bFilterByDamageType))
		TArray<TSubclassOf<class UDamageType>> DamageTypes;
	/*the priority of the Post-Process when applied to the camera*/
	UPROPERTY(EditAnywhere, Category = "First Person View|General")
		float BlendWeight = 1.0f;
	UPROPERTY(EditAnywhere, Category = "First Person View|General")
		float AnimationLength = 1.0f;
	/*the curve that handles animating of timing + intensity of the material*/
	UPROPERTY(EditAnywhere, Category = "First Person View|General")
		class UCurveFloat* AnimationCurve;

	bool HasMatchingDamageType(TSubclassOf<class UDamageType> DamageType)
	{
		/*automatically return if we have filtering off*/
		if (bFilterByDamageType == false)
			return true;
		/*otherwise if filtering is on - and we have nothing - return false cause we're dumb apparently*/
		else if (DamageTypes.Num() <= 0)
			return false;

		/*for filtering enabled - check to see if we have any damage types that match the incoming type*/
		for (uint8 i = 0; i < DamageTypes.Num(); i++)
		{
			if (DamageTypes[i] == DamageType)
				return true;
		}

		return false;
	}

	FGuid GetID() { return Guid; }
	FDamageIndicator() {}
	FDamageIndicator(UMaterialInterface* Material, TArray<TSubclassOf<UDamageType>> DamageTypes, float BlendWeight, float AnimationLength, UCurveFloat* AnimationCurve)
	{
		this->Material = Material;
		this->DamageTypes = DamageTypes;
		this->bFilterByDamageType = true;
		this->BlendWeight = BlendWeight;
		this->AnimationLength = AnimationLength;
		this->AnimationCurve = AnimationCurve;
	}
	FDamageIndicator(UMaterialInterface* Material, float BlendWeight, float AnimationLength, UCurveFloat* AnimationCurve)
	{
		this->Material = Material;
		this->DamageTypes = DamageTypes;
		this->BlendWeight = BlendWeight;
		this->AnimationLength = AnimationLength;
		this->AnimationCurve = AnimationCurve;
	}
};

USTRUCT()
struct FDamageIndicatorAnim
{
	GENERATED_BODY()

private:
	UPROPERTY()
		FGuid IndicatorId;
public:
	UPROPERTY()
		UMaterialInstanceDynamic* Material;
	UPROPERTY()
		class UCurveFloat* AnimCurve;
	UPROPERTY()
		double Length = 1.0f;
	UPROPERTY()
		double StartTimestamp;
	

	FDamageIndicatorAnim() {}
	FDamageIndicatorAnim(UObject* WorldContextObject, FGuid IndicatorGuid, UMaterialInstanceDynamic* DynamicMaterial, UCurveFloat* AnimCurve,double Length, double StartTimestamp)
	{
		this->IndicatorId = IndicatorGuid;
		this->Material = DynamicMaterial;
		this->AnimCurve = AnimCurve;
		this->Length = Length;
		this->StartTimestamp = StartTimestamp;
	}

public:
	void UpdateIntensity(UObject* WorldContextObject)
	{
		if (!Material || !AnimCurve)
			return;		
		
		Material->SetScalarParameterValue("Intensity", GetIntensity(WorldContextObject));
	}

	bool IsComplete(UObject* WorldContextObject)
	{
		/*safety check*/
		if (AnimCurve == nullptr)
			return true;

		const float TimeElapsed = GetTimeElapsed(WorldContextObject);
		return TimeElapsed >= Length;   // Length is your scalar
	}

	FGuid GetID() { return IndicatorId; }

	/*internal functions*/
private:
	float GetTimeElapsed(UObject* WorldContextObject)
	{
		return WorldContextObject->GetWorld()->GetTimeSeconds() - StartTimestamp;
	}
	float GetIntensity(UObject* WorldContextObject)
	{
		const float ElapsedTime = GetTimeElapsed(WorldContextObject);
		const float AnimTime = FMath::Clamp(ElapsedTime / Length, 0.0f, 1.0f);
		return AnimCurve->GetFloatValue(AnimTime);
	}
};

/*controls how the camera pitch/rotate is handled*/

UENUM(BlueprintType)
enum class EViewType : uint8
{
	LockedInPlace,
	VerticalFreeLook,
	FullFreeLook
};


UCLASS(ClassGroup = (FirstPerson), meta = (BlueprintSpawnableComponent), HideCategories = (Sockets, Tags, ComponentTick, ComponentReplication, Activation, Cooking, AssetUserData, Collision))
class FIRSTPERSONMODULE_API UFirstPersonViewComponent : public UActorComponent
{
	GENERATED_BODY()
public:


public:
	/*zooming*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Zooming")
		bool bAllowZoom = true;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Zooming")
		float ZoomAmount = 1.45f;		
protected:
	UPROPERTY()
		bool bZoomingIn;
	UPROPERTY()
		float CurrentZoomAmount = 1.0f;
	UPROPERTY()
		float ZoomStartAmount = 1.0f;
	UPROPERTY()
		float ZoomElapsedTime = 0.0f;
	/*how long it takes to zoom in*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Zooming")
		float ZoomInDuration = 0.135f;
	/*how long it takes to zoom out*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Zooming")
		float ZoomOutDuration = 0.2f;
	FTimerHandle ZoomAnimHandler;

private:
	UPROPERTY()
		float DefaultFOV;
	UPROPERTY()
		float CurrentFOV;

protected:
	//view height from center of root component we'll place the First Person View
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|View")
		float StandingViewHeight = 88.0f;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|View")
		float CrouchedViewHeight = 58.0f;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|View")
		float ProneViewHeight = 28.0f;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|View")
		float ViewHeightInterpSpeed = 8.0f;
	

	/*pushes the whole scene forward/backward during placement*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|View")
		float ViewForwardOffset = 0.0f;
public:
	/*allows a player to toggle "FreeLook" mode which pivots the camera while keeping the arms static*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Free Look")
		bool bAllowFreeLook = false;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Free Look")
		float VerticalFreeLookLimit = 45.0f;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Free Look")
		float HorizontalFreeLookLimit = 70.0f;
	/*how long it takes for a camera to "snap" back to normal when exiting free-look*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Free Look")
		float FreeLookReturnRate = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Free Look")
		bool bAutoCalcVerticalFreeLook = false;
protected:
	UPROPERTY()
		bool bInvertVerticalLook = false;

protected:
/*amount the character will lean when strafing left/right when walking*/
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float StrafeLean = 1.25f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float StrafeLeanInDuration = .85f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float StrafeLeanOutDuration = 0.24f;
	UPROPERTY(VisibleInstanceOnly, Category = "First Person View|Leaning")
		float CurrentStrafeLeanAlpha = 0.0f;

protected:
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float LeanDistance = 52.0f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float LeanRoll = 18.0f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float LeanInDuration = 0.28f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		float LeanOutDuration = 0.21f;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		TObjectPtr<UCurveFloat> LeanInCurve;
	UPROPERTY(EditAnywhere, Category = "First Person View|Leaning")
		TObjectPtr<UCurveFloat> LeanOutCurve;
	UPROPERTY()
		float CurrentLeanAlpha = 0.0f;
	UPROPERTY()
		float LeanStartAlpha = 0.0f;
	UPROPERTY()
		float LeanTargetAlpha = 0.0f;
	UPROPERTY()
		float LeanElapsedTime = 0.0f;

	/*animation that'll play when the player begins jumping*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "First Person View|Animations")
		class UCameraAnimationSequence* JumpAnimation;
	/*animation played when landing on the ground*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "First Person View|Animations")
		class UCameraAnimationSequence* SoftLandAnimation;
	/*animaiton to play if we hit the ground hard enough to do damage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "First Person View|Animations")
		class UCameraAnimationSequence* FallDamageAnimation;	


protected:

	/*controls how the rotate/pitch is handled (arms vs camera only)*/
	UPROPERTY()
		EViewType ViewType;
	FTimerHandle FreeLookReturnHandler;

	UPROPERTY()
		float VerticalFreeLook = 0;
	UPROPERTY()
		float Pitch = 0;
	UPROPERTY()
		float HorizontalFreeLook = 0;
	UPROPERTY()
		bool bSnapSceneToCameraOnReset = false;
	UPROPERTY()
		bool bSnapCameraToHead = false;

	/*rotation limits */



	UPROPERTY(Transient)
		TObjectPtr<class USceneComponent> FirstPersonViewRoot;
	UPROPERTY(Transient)
		TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(Transient)
		TObjectPtr<class USkeletalMeshComponent> ArmsMeshComponent;

	UPROPERTY(Transient)
		TObjectPtr<class USceneComponent> FirstPersonBodyRoot;
	
	UPROPERTY(Transient)
		TObjectPtr<class USkeletalMeshComponent> BodyMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "First Person View|Body")
		FName BodyMeshName = "FirstPersonBody";
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Body")
		TObjectPtr<USkeletalMesh> BodyMesh;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Body")
		TSubclassOf<UAnimInstance> BodyAnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Body")
		FVector BodyOffset;


	UPROPERTY(Transient)
		TObjectPtr<class USkeletalMeshComponent> LegsMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "First Person View|Legs")
		FName LegsMeshName = "FirstPersonLegs";
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Legs")
		TObjectPtr<USkeletalMesh> LegsMesh;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Legs")
		TSubclassOf<UAnimInstance> LegsAnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Legs")
		FVector LegsOffset;



	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Arms")
		USkeletalMesh* ArmsMesh;
	UPROPERTY(VisibleAnywhere, Category = "First Person View|Arms")
		FName ArmsMeshName = "FirstPersonArms";
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Arms")
		TSubclassOf<class UFirstPersonArmsAnimInstance> ArmsAnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|Arms")
		FVector ArmsOffset;

public:
	/*ui && hud*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|UI")
		TSubclassOf<class UPlayerHUDWidget> HUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|UI")
		bool bAutoShowHUD = true;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
		bool bMinimalHud = false;

protected:
	UPROPERTY()
		class UPlayerHUDWidget* PlayerHUD;
	

public:
	/*configs for damage indicator effects*/
	UPROPERTY(EditDefaultsOnly, Category = "First Person View|UI|Damage Indicators")
		TArray<FDamageIndicator> DamageIndicators;

protected:
	/*list of active damage indicator animations playing*/
	TArray<FDamageIndicatorAnim> DamageIndicatorAnimations;
	/*timer handle that runs the damage animations when active*/
	FTimerHandle DamageAnimHandler;
	
	UPROPERTY()
		AFirstPersonCharacter* OwningCharacter = nullptr;



	//===================================================================================================
	//=============================================FUNCTIONS=============================================
	//===================================================================================================
public:
	// Sets default values for this component's properties
	UFirstPersonViewComponent(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostInitProperties() override;

protected:
	virtual void InitializeComponent() override;

	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
		virtual void InitializeFirstPersonViewRoot();
	UFUNCTION()
		virtual void InitializeFirstPersonBodyRoot();
	UFUNCTION()
		virtual void InitializeCameraComponent();
	UFUNCTION()
		virtual void InitializeArmsMesh();
	UFUNCTION()
		virtual void InitializeBodyMesh();
	UFUNCTION()
		virtual void InitializeLegsMesh();
	
	UFUNCTION()
		virtual void InitializeDamageIndictators();

#if WITH_EDITOR
/*called when we hit F8 to eject from character during editor*/
	void OnSwitchPIEAndSIE(bool bIsSimulating);
#endif

/*called when this actor is being removed*/
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	/*modifies the Pawn's BaseEyeHeight variable to match the FirstPersonView*/
	UFUNCTION()
		virtual void SyncPawnEyeHeight();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "First Person View|Mesh")
		USkeletalMeshComponent* GetArmsMeshComponent() { return ArmsMeshComponent; };

	//======================================
	//================CAMERA================
	//======================================
	UFUNCTION(BlueprintPure, Category = "First Person View|Camera")
		UCameraComponent* GetCameraComponent() { return Camera; };

public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Free Looking")
		virtual void EnableFreeLook();
	UFUNCTION(BlueprintCallable, Category = "First Person View|Free Looking")
		virtual void DisableFreeLook();
	UFUNCTION(BlueprintCallable, Category = "First Person View|Free Looking")
		virtual bool IsFreeLooking() const;
	UFUNCTION(BlueprintCallable, Category = "First Person View|Free Looking")
		void SetFreeLookMode(EViewType NewMode);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "First Person View|Free Looking")
		EViewType GetFreeLookMode() { return ViewType; };
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Camera")
		void AttachCameraToHead(bool bKeepRelativeDistance = false);

	//UFUNCTION(BlueprintPure, Category = "Free Looking")
	//	bool IsFreeLooking() { return bFreeLooking; };

	UFUNCTION(BlueprintCallable, Category = "First Person View|View")
		void PitchView(float Value);
	UFUNCTION(BlueprintCallable, Category = "First Person View|View")
		void RotateView(float Value);

	/*gets the desired view height based on the character's stance & properties*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "First Person View|View")
		virtual float GetDesiredViewHeight() const;
	/*gets the desired speed at which we transition between character stances*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "First Person View|View")
		virtual float GetViewHeightInterpSpeed() const;

	//======================================
	//==============ANIMATIONS==============
	//======================================
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Animations")
		virtual void PlayCameraAnimation(class UCameraAnimationSequence* AnimationSequence);
	UFUNCTION(BlueprintCallable, Category = "First Preson View|Animations")
		virtual void PlayJumpAnimation();
	UFUNCTION(BlueprintCallable, Category = "First Person View|Animations")
		virtual void PlayLandedCameraAnimation();
	UFUNCTION(BlueprintCallable, Category = "First Person View|Animations")
		virtual void PlayFallDamageCameraAnimation();


	//=====================================
	//===============ZOOMING===============
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Zoom")
		void ZoomInVision();
	UFUNCTION(BlueprintCallable, Category = "First Person View|Zoom")
		void ZoomOutVision();
protected:
	UFUNCTION()
		void AnimateVisionZoom();


protected:
	/*function that slowly returns the view back to its original spot*/
	UFUNCTION()
		void ProcessRecenterView();

	//======================================
	//=============FIRST PERSON=============
	//======================================

public:
	UFUNCTION()
		virtual void UpdateFirstPersonVisibility();

	//=====================================
	//==========FIRST PERSON ARMS==========
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms")
		void SetFirstPersonArms(class USkeletalMesh* NewMesh);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms")
		void SetFirstPersonAnimBlueprint(TSubclassOf<class UFirstPersonArmsAnimInstance> NewAnimInstance);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms|Animations")
		virtual void PlayFullArmAnimation(class UAnimSequence* Animation);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms|Animations")
		virtual void PlayLeftArmAnimation(class UAnimSequence* Animation);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms|Animations")
		virtual void PlayRightArmAnimation(class UAnimSequence* Animation);
	//UFUNCTION(BlueprintCallable, Category = "First Person View|Arms|Animations")
	//	virtual void PlayFullArmMontage(class UAnimMontage* Montage);
		
protected:
	/*checks to see if arms should allow free looking or lock with camera*/
	UFUNCTION()
		bool ShouldArmsLockToCamera();
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Arms")
		virtual void SetArmsVisibility(bool bVisible);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Body")
		virtual void SetBodyVisibility(bool bVisible);
	UFUNCTION(BlueprintCallable, Category = "First Person View|Legs")
		virtual void SetLegsVisibility(bool bVisible);

	//===========================
	//============HUD============
	//===========================
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|UI")
		virtual void InitializePlayerHUD(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "First Person View|UI")
		virtual void ShowPlayerHUD();
	UFUNCTION(BlueprintCallable, Category = "First Person View|UI")
		virtual void HidePlayerHUD();

	/*returns the Widget instance of the HUD*/
	UFUNCTION(BlueprintPure, Category = "First Person View|UI")
		virtual UUserWidget* GetPlayerHUD();

	UFUNCTION(BlueprintPure, Category = "First Person View|Camera")
		void GetCameraView(FMinimalViewInfo& ViewInfo);

	UFUNCTION(BlueprintCallable, Category = "First Person View|UI")
		virtual void SetMinimalHUD(bool bUseMinimalHUD);


	//==========================================
	//===========DAMAGE INDICATORS==============
	//==========================================
	UFUNCTION(BlueprintCallable, Category = "First Person View|UI|Damage")
		void FlashDamageIndicator(TSubclassOf<UDamageType> DamageType);
	UFUNCTION()
		bool DamageIndicatorActive(FGuid Guid);
	UFUNCTION()
		void AddDamageIndicatorAnimation(FDamageIndicator& DamageIndicator, UMaterialInstanceDynamic*& OutMaterialInstance);
	UFUNCTION()
		void RemoveDamageIndicatorAnimation(FDamageIndicatorAnim& DamageIndicatorAnim);

	UFUNCTION()
		bool IsDamageIndicatorTimerActive();
	UFUNCTION()
		void ActivateDamageIndicatorTimer();
	UFUNCTION()
		void ClearDamageIndicatorTimer();
	UFUNCTION()
		virtual void AnimateDamageIndicator();

	UFUNCTION(BlueprintCallable, Category = "First Person View|UI|Post Process")
		void AddPostProcessMaterial(UMaterialInterface* PostProcessMaterial, float Weight = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "First Person View|UI|Post Process")
		void RemovePostProcessMaterial(UMaterialInterface* PostProcessMaterial);


	//===============================================
	//====================HELPERS====================
	//===============================================
public:
	UFUNCTION(BlueprintCallable, Category = "First Person View|Utilities")
		class AFirstPersonCharacter* GetOwningCharacter();
};
