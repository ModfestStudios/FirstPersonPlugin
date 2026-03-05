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
	UPROPERTY(VisibleAnywhere, Category = "General")
		FGuid Guid = FGuid::NewGuid();
public:
	/*the post-process material to apply to our camera*/
	UPROPERTY(EditAnywhere, Category = "General")
		class UMaterialInterface* Material;
	/*runtime Material that'll be applied to Post Processing - used for preloading*/
	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicMaterial = nullptr;
	UPROPERTY(EditAnywhere, Category = "General")
		bool bFilterByDamageType = false;
	/*the damage types to apply this for*/
	UPROPERTY(EditAnywhere, Category = "General", meta = (EditCondition = bFilterByDamageType))
		TArray<TSubclassOf<class UDamageType>> DamageTypes;
	/*the priority of the Post-Process when applied to the camera*/
	UPROPERTY(EditAnywhere, Category = "General")
		float BlendWeight = 1.0f;
	UPROPERTY(EditAnywhere, Category = "General")
		float AnimationLength = 1.0f;
	/*the curve that handles animating of timing + intensity of the material*/
	UPROPERTY(EditAnywhere, Category = "General")
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
	UPROPERTY(EditDefaultsOnly, Category = "Zooming")
		bool bAllowZoom = true;
	UPROPERTY(EditDefaultsOnly, Category = "Zooming")
		float ZoomAmount = 1.35f;
protected:
	UPROPERTY()
		bool bZoomingIn;
	UPROPERTY()
		float CurrentZoomAmount = 1.0f;
	/*how long it takes to zoom in*/
	UPROPERTY(EditDefaultsOnly, Category = "Zooming")
		float ZoomInDuration = 0.35f;
	/*how long it takes to zoom out*/
	UPROPERTY(EditDefaultsOnly, Category = "Zooming")
		float ZoomOutDuration = 0.12f;
	FTimerHandle ZoomAnimHandler;
private:
	UPROPERTY()
		float DefaultFOV;
	UPROPERTY()
		float CurrentFOV;

protected:
	//view height from center of root component we'll place the First Person View
	UPROPERTY(EditDefaultsOnly, Category = "View")
		float ViewHeight = 80.0f;
	/*pushes the whole scene forward/backward during placement*/
	UPROPERTY(EditDefaultsOnly, Category = "View")
		float ViewForwardOffset = 0.0f;
public:
	/*allows a player to toggle "FreeLook" mode which pivots the camera while keeping the arms static*/
	UPROPERTY(EditDefaultsOnly, Category = "Free Look")
		bool bAllowFreeLook = false;
	/*how long it takes for a camera to "snap" back to normal when exiting free-look*/
	UPROPERTY(EditDefaultsOnly, Category = "Free Look")
		float FreeLookReturnRate = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Free Look")
		bool bAutoCalcVerticalFreeLook = false;
protected:
	UPROPERTY()
		bool bInvertVerticalLook = false;


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

	class USceneComponent* Scene;
	class UCameraComponent* Camera;
	class USkeletalMeshComponent* Arms;

	UPROPERTY(EditDefaultsOnly, Category = "Arms")
		USkeletalMesh* ArmsMesh;
	UPROPERTY(VisibleAnywhere, Category = "Arms")
		FName ArmsMeshName = "FirstPersonArms";
	UPROPERTY(EditDefaultsOnly, Category = "Arms")
		TSubclassOf<class UFirstPersonArmsAnimInstance> AnimationBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = "Arms")
		FVector ArmsOffset;

public:
	/*ui && hud*/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UPlayerHUDWidget> HUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		bool bAutoShowHUD = true;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
		bool bMinimalHud = false;

protected:
	UPROPERTY()
		class UPlayerHUDWidget* PlayerHUD;
	

public:
	/*configs for damage indicator effects*/
	UPROPERTY(EditDefaultsOnly, Category = "UI|Damage Indicators")
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
		virtual void InitializeFirstPersonScene();
	UFUNCTION()
		virtual void InitializeCameraComponent();
	UFUNCTION()
		virtual void InitializeArmsMesh();
	UFUNCTION()
		virtual void InitializeDamageIndictators();

	/*modifies the Pawn's BaseEyeHeight variable to match the FirstPersonView*/
	UFUNCTION()
		virtual void SyncPawnEyeHeight();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Mesh")
		USkeletalMeshComponent* GetArmsMeshComponent() { return Arms; };

	//======================================
	//================CAMERA================
	//======================================
	UFUNCTION(BlueprintPure, Category = "Camera")
		UCameraComponent* GetCameraComponent() { return Camera; };

public:
	UFUNCTION(BlueprintCallable, Category = "Free Looking")
		void SetFreeLookMode(EViewType NewMode);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Free Looking")
		EViewType GetFreeLookMode() { return ViewType; };
public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
		void AttachCameraToHead(bool bKeepRelativeDistance = false);

	//UFUNCTION(BlueprintPure, Category = "Free Looking")
	//	bool IsFreeLooking() { return bFreeLooking; };

	UFUNCTION(BlueprintCallable, Category = "View")
		void PitchView(float Value);
	UFUNCTION(BlueprintCallable, Category = "View")
		void RotateView(float Value);



	//=====================================
	//===============ZOOMING===============
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "Zoom")
		void ZoomInVision();
	UFUNCTION(BlueprintCallable, Category = "Zoom")
		void ZoomOutVision();
protected:
	UFUNCTION()
		void AnimateVisionZoom();


protected:
	/*function that slowly returns the view back to its original spot*/
	UFUNCTION()
		void ProcessRecenterView();

	//=====================================
	//==========FIRST PERSON ARMS==========
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "Arms")
		void SetFirstPersonArms(class USkeletalMesh* NewMesh);
	UFUNCTION(BlueprintCallable, Category = "Arms")
		void SetFirstPersonAnimBlueprint(TSubclassOf<class UFirstPersonArmsAnimInstance> NewAnimInstance);
protected:
	/*checks to see if arms should allow free looking or lock with camera*/
	UFUNCTION()
		bool ShouldArmsLockToCamera();

	//===========================
	//============HUD============
	//===========================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializePlayerHUD(APlayerController* PlayerController);

	/*returns the Widget instance of the HUD*/
	UFUNCTION(BlueprintPure, Category = "HUD")
		virtual UUserWidget* GetPlayerHUD();

	UFUNCTION(BlueprintPure, Category = "Camera")
		void GetCameraView(FMinimalViewInfo& ViewInfo);

	UFUNCTION(BlueprintCallable, Category = "HUD")
		virtual void SetMinimalHUD(bool bUseMinimalHUD);


	//==========================================
	//===========DAMAGE INDICATORS==============
	//==========================================
	UFUNCTION(BlueprintCallable, Category = "HUD|Damage")
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

	UFUNCTION(BlueprintCallable, Category = "HUD|Post Process")
		void AddPostProcessMaterial(UMaterialInterface* PostProcessMaterial, float Weight = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "HUD|Post Process")
		void RemovePostProcessMaterial(UMaterialInterface* PostProcessMaterial);


	//===============================================
	//====================HELPERS====================
	//===============================================
public:
	UFUNCTION(BlueprintCallable, Category = "Helpers")
		class AFirstPersonCharacter* GetOwningCharacter();
};
