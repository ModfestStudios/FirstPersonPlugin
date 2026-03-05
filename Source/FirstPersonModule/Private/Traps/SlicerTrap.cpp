// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/SlicerTrap.h"

/*components*/
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ASlicerTrap::ASlicerTrap(const FObjectInitializer& ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshComp"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}
}
