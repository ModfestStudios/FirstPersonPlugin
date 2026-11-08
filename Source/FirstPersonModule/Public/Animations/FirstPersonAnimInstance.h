// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animations/Blends/FirstPersonStance.h"
#include "FirstPersonAnimInstance.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UFirstPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MovementSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FVector2D MovementDirection = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
			bool bIsMoving = false;
	// Editable in animation preview; refreshed from movement on the game thread during gameplay.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		bool bIsCrouched = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		bool bIsProne = false;




	/*character caching*/
	UPROPERTY()
		AFirstPersonCharacter* Character;
	UPROPERTY()
		UFirstPersonMovementComponent* MovementComponent;


	//=======================================================================================================================================================================================
	//=======================================================================================FUNCTIONS=======================================================================================
	//=======================================================================================================================================================================================


	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
		virtual void UpdateMovement();


	//=======================================
	//===============UTILITIES===============
	//=======================================
	// Legacy Blueprint accessors retained for existing animation assets.
	UFUNCTION(BlueprintPure, Category = "Movement", meta = (BlueprintThreadSafe))
		float GetForwardMovementDirection() const;
	UFUNCTION(BlueprintPure, Category = "Movement", meta = (BlueprintThreadSafe))
		float GetSidewayMovementDirection() const;
	UFUNCTION(BlueprintPure, Category = "Movement", meta = (BlueprintThreadSafe))
		bool IsCrouched() const;
	UFUNCTION(BlueprintPure, Category = "Movement", meta = (BlueprintThreadSafe))
		bool IsProne() const;
	UFUNCTION(BlueprintPure, Category = "Movement", meta = (BlueprintThreadSafe))
		EFirstPersonStance GetStance() const;
	UFUNCTION(BlueprintPure, Category = "Health|Death", meta = (BlueprintThreadSafe))
		bool IsDying() const;
	UFUNCTION(BlueprintPure, Category = "Weapons", meta = (BlueprintThreadSafe))
		bool HasWeaponEquipped() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
		class AFirstPersonCharacter* GetFirstPersonCharacter() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
		class UFirstPersonMovementComponent* GetFirstPersonMovement() const;
	UFUNCTION(BlueprintCallable, Category = "Initialization")
		virtual void InitializeFirstPersonCharacterReferences();
	
private:
	// Updated on the game thread; animation getters only read cached values.
	bool bCachedDying = false;
	bool bCachedWeaponEquipped = false;
};
