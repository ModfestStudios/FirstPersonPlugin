// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Combat/Combat.h"
#include "CombatSubsystem.generated.h"


//UENUM(BlueprintType)
//enum class ECombatTraceShape : uint8
//{
//	Box,
//	Sphere,
//	Capsule
//};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UCombatSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	public:


private:
	/*any attack that still exists in the world actively*/
	UPROPERTY()
		TArray<FActiveAttack> Attacks;


	//===================================================================================================================================================================================================================
	//=====================================================================================================FUNCTIONS=====================================================================================================
	//===================================================================================================================================================================================================================

public:
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCombatSubsystem, STATGROUP_Tickables); }


	/*UTickableWorldSubsystem*/
	virtual void Tick(float DeltaTime) override;



	/*runs every Tick - to process each attack as needed*/
protected:
	UFUNCTION()
		virtual void ProcessAttacks(float DeltaTime);
	

	
public:
/*call this function to initiate an attack*/
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
		virtual void InitiateAttack(class UAttackAsset* Attack, class AActor* Attacker);		
/*activates an otherwise inactive attack volume*/
	UFUNCTION()
		virtual void ActivateAttackVolume(AActor* Attacker, FName VolumeName);


protected:
	UFUNCTION()
		virtual void AddAttack(class UAttackAsset* Attack, AActor* Attacker);
	UFUNCTION()
		virtual void RemoveAttack(int32 AttackIndex);


protected:
	/*performs a sweeping trace returning all actors in that trace*/
	UFUNCTION(BlueprintCallable, Category ="Combat|Melee")
		TArray<FHitResult> PerformMeleeTrace(FVector TraceStart, FVector Direction, float Range, EAttackTraceShape CollisionShape, FVector ShapeExtent, FRotator ShapeRotation, AActor* Instigator, const TArray<AActor*>& IgnoredActors, bool bDebug = false);
	
};
