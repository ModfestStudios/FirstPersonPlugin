// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"


// Sets default values
AInventoryItem::AInventoryItem(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AInventoryItem::OnEquip_Implementation()
{
}

void AInventoryItem::OnUnequip_Implementation()
{
}

void AInventoryItem::OnPickedUp_Implementation()
{
}

