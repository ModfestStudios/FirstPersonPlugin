// Fill out your copyright notice in the Description page of Project Settings.


#include "Gore/Gib.h"

/*components*/
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGib::AGib(const FObjectInitializer& ObjectInitializer)
{
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("GibRootComp"));
	if (SceneComponent)
	{
		SetRootComponent(SceneComponent);
	}

	StaticGib = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticGibComp"));
	if (StaticGib)
	{
		StaticGib->SetupAttachment(SceneComponent);		
	}
	
	SkeletalGib = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalGibComp"));	
	if (SkeletalGib)
	{
		SkeletalGib->SetupAttachment(SceneComponent);
	}

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGib::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGib::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGib::InitializeStaticGib(UStaticMesh* StaticMesh)
{
	if (StaticMesh)
	{
		StaticGib->SetStaticMesh(StaticMesh);
		StaticGib->SetVisibility(true);
		StaticGib->SetSimulatePhysics(true);
	}

	SkeletalGib->SetSkeletalMesh(nullptr);
	SkeletalGib->SetVisibility(false);
	SkeletalGib->SetSimulatePhysics(false);
}

void AGib::InitializeSkeletalGib(USkeletalMesh* SkeletalMesh)
{
	if (SkeletalMesh)
	{
		SkeletalGib->SetSkeletalMesh(SkeletalMesh);
		SkeletalGib->SetVisibility(true);
		SkeletalGib->SetSimulatePhysics(true);
	}

	StaticGib->SetStaticMesh(nullptr);
	StaticGib->SetVisibility(false);
	StaticGib->SetSimulatePhysics(false);
}

void AGib::ApplyImpulse(FVector Impulse)
{
	if (StaticGib)
	{		
		StaticGib->AddImpulse(Impulse);
	}
}

