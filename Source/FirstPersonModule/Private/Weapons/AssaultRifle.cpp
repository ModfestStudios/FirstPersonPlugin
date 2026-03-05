// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/AssaultRifle.h"
#include "Components/InteractiveCollisionComponent.h"

AAssaultRifle::AAssaultRifle(const FObjectInitializer& ObjectIntializer)
	:Super(ObjectIntializer)
{
	if (InteractionCollision)
	{
		InteractionCollision->SetRelativeLocation(FVector(16, 0, 5));
		InteractionCollision->SetRelativeScale3D(FVector(1.4f, 0.1f, 0.3f));
	}
}
