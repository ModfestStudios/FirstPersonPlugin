// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/ThirdPersonAnimInstance.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/FirstPersonMovementComponent.h"

void UThirdPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AFirstPersonCharacter>(TryGetPawnOwner());
}

void UThirdPersonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character)
	{
		if (UFirstPersonMovementComponent* MovementComp = Cast<UFirstPersonMovementComponent>(Character->GetMovementComponent()))
		{
			/*get general movement speed*/
			if (MovementComp->Velocity.IsNearlyZero())
				MovementSpeed = 0.0f;
			else
				MovementSpeed = Character->GetMovementComponent()->Velocity.Length();

			/*get movement direction*/
			FVector velocity = MovementComp->Velocity;
			FVector forwardDir = Character->GetActorForwardVector();
			FVector sidewayDir = Character->GetActorRightVector();
			MovementDirection.X = FVector::DotProduct(velocity.GetSafeNormal2D(), forwardDir.GetSafeNormal2D());			
			MovementDirection.Y = FVector::DotProduct(velocity.GetSafeNormal2D(), sidewayDir.GetSafeNormal2D());
			
		}
		
	}
}

void UThirdPersonAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);
}

bool UThirdPersonAnimInstance::IsMoving()
{
	if (!Character)
		return PF_bMoving; //returns our "Preview" flag for moving visualization
	else if (MovementSpeed > 0)
		return true;
	else
		return false;
	
}

float UThirdPersonAnimInstance::GetMovementSpeed()
{
	return MovementSpeed;
}

float UThirdPersonAnimInstance::GetForwardMovementDirection()
{
	return MovementDirection.X * 100;
}

float UThirdPersonAnimInstance::GetSidewayMovementDirection()
{
	return MovementDirection.Y * 100;
}

bool UThirdPersonAnimInstance::IsDying()
{
	return bDying;
}

void UThirdPersonAnimInstance::OnDeath(const FDeathInfo DeathInfo)
{
	bDying = true;
}
