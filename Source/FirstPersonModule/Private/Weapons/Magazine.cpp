// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Magazine.h"

/*components*/
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AMagazine::AMagazine(const FObjectInitializer& ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MagazineComp"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}
 	
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMagazine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

