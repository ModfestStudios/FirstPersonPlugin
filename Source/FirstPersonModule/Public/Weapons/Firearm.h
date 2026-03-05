// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Subsystems/ProjectileSubsystem.h"
#include "Firearm.generated.h"


UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Single,
	Semi,
	Burst,
	FullAuto
};

/**
 *
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AFirearm : public AWeapon
{
	GENERATED_BODY()
public:

	/*firerates & modes*/
protected:
	UPROPERTY(Replicated)
		bool bFiring = false;
	UPROPERTY(EditDefaultsOnly, Category = "Fire Modes", meta = (DisplayName = "Default FireMode"))
		EFireMode FireMode = EFireMode::Semi;
	UPROPERTY(EditAnywhere, Category = "Fire Mode")
		float FireRate = 600.0f;
	UPROPERTY()
		float ShotsFired = 0;
private:
	FTimerHandle FireIntervalTimer;


	/*bullet-spread*/
	UPROPERTY(Replicated)
		uint32 ProjectileSpreadSeed; //used to create bullet-spread that's reliable across network
	FRandomStream ProjectileSpreadStream; //used to create a stream of "random" numbers fed in by the replicated seed above


private:
	UPROPERTY(VisibleAnywhere, Category = "Projectiles")
		FName ProjectileSocketName = FName("MuzzleSocket");
public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectiles")
	//	class UProjectileSpawnerComponent* ProjectileSpawnerComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles")
		class UProjectileInfo* ProjectileType;
	UPROPERTY()
		UProjectileSubsystem* ProjectileSubsystem;

	UPROPERTY()
		USkeletalMeshComponent* MagazineMesh;

	//=====================================================================================
	//=====================================FUNCTIONS=======================================
	//=====================================================================================
public:
	AFirearm(const FObjectInitializer& ObjectInitializer);



	//================================
	//=============FIRING=============
	//================================
	virtual void BeginFire() override;
	virtual void EndFire() override;

protected:
	UFUNCTION()
		virtual void Fire();
	UFUNCTION()
		virtual void SpawnProjectile();

public:
	UFUNCTION(BlueprintPure, Category = "Weapon|Firing")
		bool IsFiring();
	UFUNCTION(BlueprintPure, Category = "Weapon|Firing")
		bool CanFire();





	//=================================
	//============EQUIPPING============
	//=================================
	virtual void OnEquip_Implementation() override;





	//=======================================
	//==============PROJECTILES==============
	//=======================================
public:
	UFUNCTION()
		UProjectileSubsystem* GetProjectileSubsystem();
	UFUNCTION()
		virtual void GetProjectileSpawnLocation(FVector& Location, FVector& Direction);
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectiles")
		class UProjectileInfo* GetProjectileType();




};
