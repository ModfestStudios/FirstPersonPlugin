// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Blueprints/TrapAnimInstance.h"
#include "Traps/TrapBase.h"

void UTrapAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	
}

void UTrapAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (ATrap* Trap = GetOwningTrap())
	{
		bTrapActivated = Trap->IsTrapActivated();
		bRetracted = Trap->IsTrapRetracted();
	}
}

void UTrapAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

ATrap* UTrapAnimInstance::GetOwningTrap()
{
	AActor* OwningActor = GetOwningActor();
	if (ATrap* OwningTrap = Cast<ATrap>(OwningActor))
		return OwningTrap;
	else
		return nullptr;
}
