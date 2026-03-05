// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoreComponent.generated.h"

UENUM(BlueprintType)
enum class EGibSide : uint8
{
	Left,
	Right
};
UENUM(BlueprintType)
enum class EArmGibType : uint8
{
	FullArm,
	UpperArm,
	LowerArm,
	Hand,
	Finger
};
UENUM(BlueprintType)
enum class ELegGibType : uint8
{
	FullLeg,
	UpperLeg,
	LowerLeg,
	Foot
};

USTRUCT(BlueprintType)
struct FHeadGib 
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Gibs")
		UStaticMesh* GibMesh;

	/*if system decides to gib an eyeball - will ignore appropriately*/
	UPROPERTY(EditAnywhere, Category = "Gibs")
		bool bLeftEyeAttached = true;
	/*if system decides to gib an eyeball - will ignore appropriately*/
	UPROPERTY(EditAnywhere, Category = "Gibs")
		bool bRightEyeAttached = true;

};

USTRUCT(BlueprintType)
struct FArmGib
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Gibs")
		class UStaticMesh* GibMesh;
	UPROPERTY(EditAnywhere, Category = "Gibs")
		EGibSide SideOfBody = EGibSide::Left;
	UPROPERTY(EditAnywhere, Category = "Gibs")
		EArmGibType GibType = EArmGibType::FullArm;
};

USTRUCT(BlueprintType)
struct FLegGib
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Gibs")
		class UStaticMesh* GibMesh;
	UPROPERTY(EditAnywhere, Category = "Gibs")
		EGibSide SideOfBody = EGibSide::Left;
	UPROPERTY(EditAnywhere, Category = "Gibs")
		ELegGibType GibType = ELegGibType::FullLeg;
};


UCLASS( ClassGroup=(Gore), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UGoreComponent : public UActorComponent
{
	GENERATED_BODY()
public:


	/*generic gib bits that can be randomly spawned*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Generic")
		TArray<class UStaticMesh*> Gibs;

	/*upper body - including arms and head*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body")
		TArray<class USkeletalMesh*> UpperBody;
	/*lower body - pelvis and legs basically*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body")
		TArray<class USkeletalMesh*> LowerBody;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body Parts")
		TArray<FHeadGib> HeadGibs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body Parts")
		TArray<class UStaticMesh*> EyeGibs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body Parts")
		TArray<class UStaticMesh*> ChestGibs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body Parts")
		TArray<FArmGib> ArmGibs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gibbing|Body Parts")
		TArray<FLegGib> LegGibs;


	//===========================================================================================================================
	//=========================================================FUNCTIONS=========================================================
	//===========================================================================================================================

public:	
	// Sets default values for this component's properties
	UGoreComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
