// Copyrighted : Modfest Studios 2025-2026


#include "Animations/FirstPersonAnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Characters/FirstPersonCharacter.h"
#include "Weapons/Weapon.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/PlayerInventoryManagerComponent.h"
#include "Components/FirstPersonMovementComponent.h"



void UFirstPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	InitializeFirstPersonCharacterReferences();
}

void UFirstPersonAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!IsValid(Character) || !IsValid(MovementComponent))
		InitializeFirstPersonCharacterReferences();

	bCachedDying = IsValid(Character) && Character->IsPlayerDead();
	bCachedWeaponEquipped = IsValid(Character) && Character->HasWeaponEquipped();
	UpdateMovement();
}

void UFirstPersonAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);	


}

void UFirstPersonAnimInstance::UpdateMovement()
{
	const UWorld* World = GetWorld();
	const bool bHasMovementComponent = IsValid(Character) && IsValid(MovementComponent);

	// Keep manually edited Property Preview values, even when the preview has no pawn owner.
	if (!World || World->WorldType != EWorldType::EditorPreview)
	{
		bIsCrouched = bHasMovementComponent && MovementComponent->IsCrouching();
		bIsProne = bHasMovementComponent && MovementComponent->IsProne();
	}

	if (!bHasMovementComponent)
		return;

	const FVector Velocity = MovementComponent->Velocity;;
	const FVector GroundVelocity(Velocity.X,Velocity.Y, 0.0f);

	MovementSpeed = GroundVelocity.Size();
	bIsMoving = MovementSpeed > KINDA_SMALL_NUMBER;

	if (bIsMoving)
	{
		const FVector LocalVelocity = Character->GetActorTransform().InverseTransformVectorNoScale(GroundVelocity);
		MovementDirection = FVector2D(LocalVelocity.X,LocalVelocity.Y).GetSafeNormal();
	}
	else
	{
		MovementDirection = FVector2D::ZeroVector;
	}
}


//=======================================
//===============UTILITIES===============
//=======================================

float UFirstPersonAnimInstance::GetForwardMovementDirection() const
{
	return MovementDirection.X * 100.0f;
}

float UFirstPersonAnimInstance::GetSidewayMovementDirection() const
{
	return MovementDirection.Y * 100.0f;
}

bool UFirstPersonAnimInstance::IsCrouched() const
{
	return bIsCrouched;
}

bool UFirstPersonAnimInstance::IsProne() const
{
	return bIsProne;
}

EFirstPersonStance UFirstPersonAnimInstance::GetStance() const
{
	if (bIsProne)
		return EFirstPersonStance::Prone;
	if (bIsCrouched)
		return EFirstPersonStance::Crouched;

	return EFirstPersonStance::Standing;
}

bool UFirstPersonAnimInstance::IsDying() const
{
	return bCachedDying;
}

bool UFirstPersonAnimInstance::HasWeaponEquipped() const
{
	return bCachedWeaponEquipped;
}
AFirstPersonCharacter* UFirstPersonAnimInstance::GetFirstPersonCharacter() const
{
	return Character;
}

UFirstPersonMovementComponent* UFirstPersonAnimInstance::GetFirstPersonMovement() const
{
	return MovementComponent;
}

void UFirstPersonAnimInstance::InitializeFirstPersonCharacterReferences()
{
	Character = Cast<AFirstPersonCharacter>(TryGetPawnOwner());

	MovementComponent = IsValid(Character) ? Character->GetCharacterMovement<UFirstPersonMovementComponent>() : nullptr;
}
