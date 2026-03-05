// Copyright (c) 2022 Pocket Sized Animations

/*First Person Arms Anim Instance - This class is designed to be more of a template than an out-of-the-box solution.
* it provides a lot of functions you can override to make a first person setup easier
* check out ExtendedFirstPersonModule for a version that uses most of the other Endurance Modules (used in games such as Black Sierra and Tier One) for a more out of the box approach
* (assuming you're using those modules)
*
*/

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonArmsAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAnimState_ItemState : uint8
{
	NoCurrentItem,
	Equipping,
	Equipped,
	Unequipping
};

UENUM(BlueprintType)
enum class EAnimState_MovementState : uint8
{
	NotMoving,
	Walking,
	Running,
	Sprinting,
	Swimming,
	Jumping
};

/**
 *
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UFirstPersonArmsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:



	/*inventory*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		AActor* CurrentlyEquippedItem;
	UPROPERTY()
		EAnimState_ItemState CurrentItemState = EAnimState_ItemState::NoCurrentItem;

	/*movement*/
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		bool bMoving = false;
	UPROPERTY()
		EAnimState_MovementState MovementState = EAnimState_MovementState::NotMoving;
	UPROPERTY()
		float MovementVelocity;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement")
		float SlowWalkSpeed = 3.0f;
	/*speed at which player is considered "walking"*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement")
		float WalkSpeed = 150.0f;
	/*speed at which player is considered "running"*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement")
		float RunSpeed = 610.0f;
	/*speed at which playerr is considered "sprining"*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement")
		float SprintSpeed = 780.0f;


	UPROPERTY()
		AFirstPersonCharacter* CachedCharacter;

	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;



	//=====================================================
	//=======================INVENTORY=======================
	//=====================================================
	/*override this function to extend into your own wepaon logic*/
	UFUNCTION()
		virtual void UpdateInventoryStates();


	UFUNCTION(BlueprintPure, Category = "Inventory", meta = (BlueprintThreadSafe))
		virtual bool IsEquippingItem();
	UFUNCTION(BlueprintPure, Category = "Inventory", meta = (BlueprintThreadSafe))
		virtual bool HasItemEquipped();
	UFUNCTION(BlueprintPure, Category = "Inventory", meta = (BlueprintThreadSafe))
		virtual bool IsUnequippingItem();

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapons", meta = (NotBlueprintThreadSafe))
		virtual bool HasWeaponEquipped();

	//==============================================
	//===================MOVEMENT===================
	//==============================================
	UFUNCTION(meta = (BlueprintThreadSafe))
		virtual void UpdateMovementState();
	UFUNCTION(BlueprintPure, category = "Movement", meta = (BlueprintThreadSafe))
		virtual bool IsMoving();
	UFUNCTION(BlueprintPure, category = "Movement", meta = (BlueprintThreadSafe))
		virtual bool IsRunning();
	UFUNCTION(BlueprintPure, category = "Movement", meta = (BlueprintThreadSafe))
		virtual bool IsSprinting();



	//=======================================
	//===============UTILITIES===============
	//=======================================
	UFUNCTION()
		class AFirstPersonCharacter* GetFirstPersonCharacter();
};
