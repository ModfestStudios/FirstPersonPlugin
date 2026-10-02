// Copyrighted : Modfest Studios 2025-2026


#include "Animations/FirstPersonAnimInstance.h"
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

	if(!Character)
		InitializeFirstPersonCharacterReferences();

	UpdateMovement();
}

void UFirstPersonAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);	


}

void UFirstPersonAnimInstance::UpdateMovement()
{
	if(!Character || !MovementComponent)
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

	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement<UFirstPersonMovementComponent>();
	}
}
