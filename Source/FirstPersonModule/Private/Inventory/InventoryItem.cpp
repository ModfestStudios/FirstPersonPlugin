// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Inventory/Inventory.h"


// Sets default values
AInventoryItem::AInventoryItem(const FObjectInitializer& ObjectInitializer)
{
	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UInventoryItemComponent>(this, TEXT("ItemComp"));
	if (ItemComponent)
	{
		AddOwnedComponent(ItemComponent);

		ItemComponent->bAutoEquip = false;
	}

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

FText AInventoryItem::GetItemName()
{
	return GetItemComponent()->GetItemName();
}

UMaterialInterface* AInventoryItem::GetItemIcon()
{
	return ItemComponent->GetItemIcon();
}

EItemSize AInventoryItem::GetItemSize()
{
	if (ItemComponent)
		return ItemComponent->GetItemSize();
	else
		return EItemSize::Small;
}

EItemPresence AInventoryItem::GetItemPresence()
{
	return ItemComponent->GetItemPresence();
}

bool AInventoryItem::CanBePickedUp()
{
	if (ItemComponent->GetItemPresence() != EItemPresence::World)
		return false;

	return true;
}

void AInventoryItem::OnItemStored(AActor* StorageActor)
{
	if (!StorageActor)
		return;

	UInventoryManagerComponent* InventoryManager = StorageActor->GetComponentByClass<UInventoryManagerComponent>();

	ItemComponent->SetItemPresence(EItemPresence::Storage);

}

void AInventoryItem::OnItemPickedUp(AActor* ReceivingActor)
{

}

void AInventoryItem::OnItemDropped(AActor* RejectingActor)
{

}


UInventoryItemComponent* AInventoryItem::GetItemComponent()
{
	return ItemComponent;
}


//=======================
//======ATTACHMENTS======
//=======================

void AInventoryItem::OnItemAttached(AInventoryItem* Item, UInventoryAttachmentComponent* AttachedComponent)
{

}

//=====================
//=======MANAGER=======
//=====================

UInventoryManagerComponent* AInventoryItem::GetInventoryManager()
{
	return GetComponentByClass<UInventoryManagerComponent>();
}


//====================
//========GRID========
//====================
void AInventoryItem::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
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

