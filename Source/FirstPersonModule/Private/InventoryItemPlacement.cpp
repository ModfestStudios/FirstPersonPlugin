// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemPlacement.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AInventoryItemPlacement::AInventoryItemPlacement(const FObjectInitializer& ObjectInitializer)
{
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	if (SceneComponent)
	{
		SetRootComponent(SceneComponent);
	}

	UpAxisComponent = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("UpAxis"));

	if (UpAxisComponent)
	{
		UpAxisComponent->SetupAttachment(SceneComponent);
		UpAxisComponent->SetArrowFColor(FColor(0, 0, 255, 255));
		UpAxisComponent->SetWorldRotation(FRotator(90, 0, 0));
		UpAxisComponent->ArrowSize = 0.5f;
		UpAxisComponent->ArrowLength = 50.0f;
	}

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AInventoryItemPlacement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryItemPlacement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

