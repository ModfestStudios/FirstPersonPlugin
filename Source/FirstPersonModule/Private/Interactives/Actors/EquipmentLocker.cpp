// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/EquipmentLocker.h"

/*players*/
#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"

#include "UI/Widgets/LoadoutWidget.h"

// Sets default values
AEquipmentLocker::AEquipmentLocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipmentLocker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquipmentLocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquipmentLocker::OpenLockerWidget(AFirstPersonPlayerController* Player)
{
	if (Player && Player->IsLocalController())
	{
		Player->InitializeLoadoutWidget(LoadoutWidgetScreen);
		Player->OpenLoadoutWidget();
	}
}

