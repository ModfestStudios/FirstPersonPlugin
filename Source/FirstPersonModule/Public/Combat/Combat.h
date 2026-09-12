// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Combat.generated.h"


UENUM(BlueprintType)
enum class ESetTargetType : uint8
{
	ClosestThreat,
	HighestThreat,
	LowestThreat,
	BlackboardKey
};

UENUM(BlueprintType)
enum class EAttackCategory : uint8
{
	Melee,
	Ranged,
	Special,
	Attack
};


UENUM()
enum class ECheckTargetType : uint8
{
	AIController,
	BlackboardKey
};

USTRUCT(BlueprintType)
struct FAttackCheckCriteria
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Criteria")
		bool bCheckRange = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Criteria")
		bool bCheckCooldown = true;
};


UENUM(BlueprintType)
enum class EAttackTraceShape : uint8
{
	Box,
	Sphere,
	Capsule,
	Cone
};

UENUM(BlueprintType)
enum class EAttackTiming : uint8
{
	Timed,
	AnimEvent
};

USTRUCT(BlueprintType)
struct FAttackHitVolume
{
	GENERATED_BODY()

public:

	/*used to help ID the volume when activating/deactiving it*/
	UPROPERTY(EditDefaultsOnly)
		FName VolumeName = NAME_None;
	UPROPERTY(EditDefaultsOnly)
		EAttackTraceShape TraceShape = EAttackTraceShape::Box;

	UPROPERTY(EditDefaultsOnly)
		FRotator ShapeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly)
		FVector ShapeExtent = FVector(45.0f, 45.0f, 90.0f);

	UPROPERTY(EditDefaultsOnly)
		FVector Direction = FVector(1.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly)
		FVector StartOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly)
		float Range = 200.0f;

	UPROPERTY(EditDefaultsOnly)
		EAttackTiming AttackTiming = EAttackTiming::Timed;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "AttackTiming == EAttackTiming::Timed", EditConditionHides))
		float StartDelay = 0.0f;

	UPROPERTY(EditDefaultsOnly)
		float Duration = 0.35f;
};

USTRUCT()
struct FActiveAttackVolume
{
	GENERATED_BODY()

		UPROPERTY()
		FAttackHitVolume Volume;

	UPROPERTY()
		bool bActivated = false;

	UPROPERTY()
		float ActivationTime = -1.0f;
};

USTRUCT(BlueprintType)
struct FActiveAttack
{
	GENERATED_BODY()

public:

	UPROPERTY()
		AActor* Attacker = nullptr;

	UPROPERTY()
		class UAttackAsset* AttackAsset = nullptr;

	UPROPERTY()
		float ExecutionTimestamp = -1.0f;

	UPROPERTY()
		float ElapsedTime = 0.0f;

	UPROPERTY()
		bool bDebugging = false;

	UPROPERTY()
		TArray<FActiveAttackVolume> HitVolumes;

	UPROPERTY()
		TSet<TWeakObjectPtr<AActor>> ActorsHit;
};
