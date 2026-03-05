// Copyright (c) 2022 Pocket Sized Animations


#include "Animations/FirstPersonArmsAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Characters/FirstPersonCharacter.h"
#include "Weapons/Weapon.h"
#include "Components/InventoryManagerComponent.h"



void UFirstPersonArmsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UFirstPersonArmsAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

}

void UFirstPersonArmsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateMovementState();
	UpdateInventoryStates();
}


//=====================================================
//=======================INVENTORY=======================
//=====================================================

void UFirstPersonArmsAnimInstance::UpdateInventoryStates()
{
	if (GetFirstPersonCharacter())
	{
		if (UInventoryManagerComponent* InvManager = GetFirstPersonCharacter()->GetInventoryManager())
		{
			CurrentlyEquippedItem = InvManager->GetCurrentlyEquippedItem();
		}
	}
}


bool UFirstPersonArmsAnimInstance::IsEquippingItem()
{
	return false;
}

bool UFirstPersonArmsAnimInstance::HasItemEquipped()
{
	return IsValid(CurrentlyEquippedItem);
}

bool UFirstPersonArmsAnimInstance::IsUnequippingItem()
{
	return false;
}

bool UFirstPersonArmsAnimInstance::HasWeaponEquipped()
{
	if (!HasItemEquipped())
		return false;

	if (CurrentlyEquippedItem->IsA<AWeapon>())
		return true;

	return false;
}

//============================================================================
//==================================MOVEMENT==================================
//============================================================================

void UFirstPersonArmsAnimInstance::UpdateMovementState()
{
	if (TryGetPawnOwner() && TryGetPawnOwner()->GetMovementComponent())
	{
		MovementVelocity = TryGetPawnOwner()->GetMovementComponent()->Velocity.Size();
		if (MovementVelocity > 0)
		{
			bMoving = true;

			if (MovementVelocity > SprintSpeed)
				MovementState = EAnimState_MovementState::Sprinting;
			if (MovementVelocity > RunSpeed)
				MovementState = EAnimState_MovementState::Running;
			else if (MovementVelocity > WalkSpeed)
				MovementState = EAnimState_MovementState::Walking;
		}

		else
		{
			bMoving = false;
			MovementState = EAnimState_MovementState::NotMoving;
		}
	}
}

bool UFirstPersonArmsAnimInstance::IsMoving()
{
	return bMoving;
}

bool UFirstPersonArmsAnimInstance::IsRunning()
{
	if (MovementState == EAnimState_MovementState::Running)
		return true;

	return false;
}

bool UFirstPersonArmsAnimInstance::IsSprinting()
{
	if (MovementState == EAnimState_MovementState::Sprinting)
		return true;

	return false;
}

AFirstPersonCharacter* UFirstPersonArmsAnimInstance::GetFirstPersonCharacter()
{
	if (CachedCharacter == nullptr)
		CachedCharacter = Cast<AFirstPersonCharacter>(TryGetPawnOwner());

	return CachedCharacter;
}

