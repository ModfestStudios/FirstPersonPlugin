// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/TerminalSystem.h"

/*components*/
#include "Components/WidgetComponent.h"
#include "Components/InteractiveCollisionComponent.h"


// Sets default values
ATerminalSystem::ATerminalSystem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{


	Widget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	if (Widget)
	{
		Widget->SetupAttachment(GetRootComponent());
	}

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATerminalSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerminalSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

