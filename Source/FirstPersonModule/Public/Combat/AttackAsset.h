// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Combat.h"
#include "AttackAsset.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UAttackAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:


	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		FName Name = FName("Generic Attack");
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		EAttackCategory Category = EAttackCategory::Melee;

		/*the min/max range of the attack*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		FVector2D Range = FVector2D(50, 150);
		/*cooldown time before this attack can be used again*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		float Cooldown = 0.5f;
		/*how much stamina does this action cost*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		float StaminaDrain = 23.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		bool bDebugAttack = false;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
		TArray<FAttackHitVolume> HitVolumes;



		/*the animation montage that'll play this attack*/
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		TObjectPtr<class UAnimMontage> AttackAnimation;
	
};
