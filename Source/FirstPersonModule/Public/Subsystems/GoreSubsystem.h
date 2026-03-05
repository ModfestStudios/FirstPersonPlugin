// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GoreSubsystem.generated.h"

UENUM(BlueprintType)
enum class EGibCategory : uint8
{
	None
};

USTRUCT(BlueprintType)
struct FGoreDecal
{
	GENERATED_BODY()
public:

	UPROPERTY()
		class UDecalComponent* Decal;
	UPROPERTY()
		bool bPermanentDecal;
	UPROPERTY()
		float Lifetime;
	UPROPERTY()
		float ElapsedTime;

	FGoreDecal() : Decal(nullptr), bPermanentDecal(false), Lifetime(300.0f), ElapsedTime(0.0f) {}

};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UGoreSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:

	
	UPROPERTY()
		TArray<FGoreDecal> ActiveDecals;
	UPROPERTY()
		TArray<class AGib*> ActiveGibs;

		
	const float DefaultDecalHeight = 10.0f;


	//=================================================================================================================
	//====================================================FUNCTIONS====================================================
	//=================================================================================================================

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGoreSubsystem, STATGROUP_Tickables); }
	virtual void Tick(float DeltaTime) override;


	
	UFUNCTION(BlueprintCallable, Category = "Gibs")
		void SpawnStaticGib(UStaticMesh* Mesh, FVector Location, FVector Impulse, float Scale = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "Gibs")
		void SpawnSkeletalGib(USkeletalMesh* Mesh, FVector Location, FVector Impulse, float Scale = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "Gore")
		FGoreDecal SpawnGoreDecal(class UMaterialInterface* GoreMaterial,FVector Location, FVector SurfaceNormal = FVector(0,0,1),float Size = 150.0f,bool bPermanentDecal = false, float Lifetime = 320.0f);

	
	
};
