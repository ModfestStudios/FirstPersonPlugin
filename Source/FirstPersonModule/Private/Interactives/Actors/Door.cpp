// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/Door.h"
#include "Components/InteractiveCollisionComponent.h"
#include "Components/DoorComponent.h"
#include "Interactives/InteractiveAction.h"

// Sets default values
ADoor::ADoor(const FObjectInitializer& ObjectInitializer)
{	

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;	
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	DoorComponents.Empty();
	GetComponents<UDoorComponent>(DoorComponents);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::Open()
{
	for (UDoorComponent* Door : DoorComponents)
	{
		if (Door)
			Door->OpenDoor();
	}
}

void ADoor::Close()
{
	for (UDoorComponent* Door : DoorComponents)
	{
		if (Door)
			Door->CloseDoor();
	}
}

