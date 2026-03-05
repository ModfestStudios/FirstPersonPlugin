// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/CrusherTrap.h"

/*components*/
#include "Components/SkeletalMeshComponent.h"

ACrusherTrap::ACrusherTrap(const FObjectInitializer& ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshComp"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}

}
