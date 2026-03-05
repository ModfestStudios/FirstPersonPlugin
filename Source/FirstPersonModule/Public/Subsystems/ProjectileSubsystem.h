// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Projectiles/ProjectileInfo.h"
#include "WorldCollision.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/HitResult.h"

#include "ProjectileSubsystem.generated.h"


USTRUCT(BlueprintType)
struct FProjectile
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		class UProjectileInfo* ProjectileInfo;
	/*direction info*/
	UPROPERTY()
		FVector Location;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		FVector PreviousLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		FVector ForwardDirection;
	/*speed info*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float Velocity; //meters per second
	/*gravity info*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float GravityStrength;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float TerminalFallVelocity;
	/*tracer bits*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float TracerVelocity;
	/*travel-expenses*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float DistanceTraveled;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float EffectiveRange;

	/*impact info*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		bool bProjectileHit = false;
	bool bHitProcessed = false;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		FHitResult HitResults;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		float TimeSinceHit;

	TArray<AActor*> IgnoredActors;

	/*instigator info*/
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		class AActor* DamageCauser;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		class APawn* OwningPawn;
	UPROPERTY(BlueprintReadOnly, Category = "Projectiles")
		class AController* Instigator;

	FProjectile() {};
	FProjectile(UProjectileInfo* _projectileInfo, FVector _location, FVector _direction, AActor* _damageCauser, APawn* _owningPawn, AController* _instigator) :
		ProjectileInfo(_projectileInfo),
		Location(_location),
		ForwardDirection(_direction),
		DamageCauser(_damageCauser),
		OwningPawn(_owningPawn),
		Instigator(_instigator)
	{
		/*configure info based on ProjectileInfo*/
		if (ProjectileInfo)
		{
			Velocity = ProjectileInfo->Velocity * 100.0f;
			EffectiveRange = ProjectileInfo->EffectiveRange * 100.0f;
		}
		/*fallback if ProjectileInfo is blank - use generic info*/
		else
		{
			Velocity = 922.0f * 100.0f;
			EffectiveRange = 800.0f * 100.0f;
		}

		/*the amount of gravity being applied currently*/
		GravityStrength = 0;
		/*what's the max rate of downward "fall" (gravity) can we have*/
		TerminalFallVelocity = 920.0f;

		/*setup collision ignorance to avoid "shooting" ourselves*/
		if (DamageCauser)
			IgnoredActors.Add(DamageCauser);
		if (OwningPawn)
			IgnoredActors.Add(OwningPawn);
	};


	float GetDamage() { return ProjectileInfo != nullptr ? ProjectileInfo->GetDamage() : 25.0f; };
};

/*DELEGATES DEFINITIONS*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHit, FProjectile, Projectile);

/**
 *
 */
UCLASS()
class FIRSTPERSONMODULE_API UProjectileSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "Projectiles")
		TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
	const float Gravity = 980.0f;

	UPROPERTY()
		TArray<FProjectile> Projectiles;

	FCollisionQueryParams ProjectileCollisionParams;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Projectiles")
		bool bDebugProjectiles = false;

	/*events/delegates*/
	UPROPERTY(BlueprintAssignable, Category = "Projectiles")
		FOnProjectileHit OnProjectileHit;



	//===================================================================================================
	//=============================================FUNCTIONS=============================================
	//===================================================================================================

	UProjectileSubsystem();

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;


public:
	UFUNCTION()
		void SpawnProjectile(FProjectile& NewProjectile);

	//=================================================
	//==============PROJECTILE PROCESSING==============
	//=================================================
private:
	UFUNCTION()
		void ProcessProjectileMovement(FProjectile& Projectile, float DeltaTime);
	UFUNCTION()
		void ApplyGravity(FProjectile& Projectile, FVector& MovementUpdate, float DeltaTime);
	UFUNCTION()
		void ProcessProjectileHit(FProjectile& Projectile);
	UFUNCTION()
		bool IsProjectileBeyondEffectiveRange(FProjectile& Projectile);
	UFUNCTION()
		void UpdateCollisionParamsIgnoredActorsList(FProjectile& Projectile);


	FColor GetTraceDebugColor(FProjectile& Projectile);
};
