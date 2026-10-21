// Copyright (c) 2022 Pocket Sized Animations


#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Inventory/InventoryItem.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/InventoryAttachmentComponent.h"
//#include "Events/ItemEvent.h"

/*interface*/
#include "Interfaces/InventoryItemInterface.h"
#include "Interfaces/PrimaryWeaponInterface.h"
#include "Interfaces/SecondaryWeaponInterface.h"
#include "Interfaces/AlternativeWeaponInterface.h"

/*engine*/
#include "Engine/World.h"

/*framework*/
#include "GameFramework/Actor.h"

/*subsystems*/
#include "Subsystems/LootSubsystem.h"
#include "Subsystems/InventorySubsystem.h"

/*replication*/
#include "Net/UnrealNetwork.h"

/*utilities*/
#include "TimerManager.h"

void UInventoryItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemComponent, bCollisionEnabled);
    DOREPLIFETIME(UInventoryItemComponent, InventoryManager);
    DOREPLIFETIME(UInventoryItemComponent, AttachmentSlot);
    DOREPLIFETIME(UInventoryItemComponent, ItemPresence);
    DOREPLIFETIME(UInventoryItemComponent, EquipState);
}

// Sets default values for this component's properties
UInventoryItemComponent::UInventoryItemComponent()
{
	/*replication*/
	SetIsReplicatedByDefault(true);
	/*ticking*/
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UInventoryItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	RegisterInventoryItem();
}

//================================================
//====================DETAILS=====================
//================================================

FText UInventoryItemComponent::GetItemName() const
{
	return ItemName;
}

FText UInventoryItemComponent::GetItemDescription() const
{
	return ItemDescription;
}

UMaterialInterface* UInventoryItemComponent::GetItemIcon() const
{
	switch (Condition)
	{
		case EItemCondition::Prestine:
			return PrestineItemIcon;
		case EItemCondition::Worn:
			return WornItemIcon;
		case EItemCondition::Damaged:
			return DamagedItemIcon;
		case EItemCondition::BadlyDamaged:
			return BadlyDamageItemIcon;
		case EItemCondition::Ruined:
			return RuinedItemIcon;
		default:
			return PrestineItemIcon;
	}

}

EItemSize UInventoryItemComponent::GetItemSize() const
{
	return Size;
}

//================================================
//======================POS=======================
//================================================

bool UInventoryItemComponent::IsSellable()
{
	return bSellableItem;
}

//================================================
//=====================STATES=====================
//================================================

EItemPresence UInventoryItemComponent::GetItemPresence()
{
	return ItemPresence;
}

void UInventoryItemComponent::SetItemPresence(EItemPresence NewPresence)
{
	ItemPresence = NewPresence;
}

void UInventoryItemComponent::OnRep_ItemPresence()
{
    if (!GetOwner())
        return;

    AActor* Holder = IsValid(InventoryManager) ? InventoryManager->GetOwner() : nullptr;

    if (ItemPresence != EItemPresence::World && !IsValid(Holder))
        return;
    if (ItemPresence == EItemPresence::Attached && (EquipState != EEquipState::Equipped || !AttachmentSlot))
        return;
    if (LastNotifiedPresence == ItemPresence && LastNotifiedHolder.Get() == Holder)
        return;
    LastNotifiedPresence = ItemPresence;
    LastNotifiedHolder = Holder;
    if (IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(GetOwner()))
    {
        if (ItemPresence == EItemPresence::Storage) Interface->OnItemStored(Holder);
        else if (ItemPresence == EItemPresence::Attached)
            Interface->StopEquipping(nullptr, this, InventoryManager, EInventoryItemOperationEndReason::Completed);
        else Interface->OnItemDropped(nullptr);
    }
}

//================================================
//===================EQUIPPING====================
//================================================

/*BeginEquip() - Called by the InventoryManager as a generic entry for any Inventory Item to provide an easy universal way across different Actor-types to receive
* equipping notifications without needing to extend from a base case
* Actor lifecycle notifications are dispatched through InventoryItemInterface.
* It's ASSUMED the Actor will then notify this ItemComponent it's completed via EndEquip() so it can notify the InventoryManager and update the Items State
*/
void UInventoryItemComponent::BeginEquip(AFirstPersonCharacter* User)
{
	if (EquipState == EEquipState::Equipping)
		return;
    EquipRequestUser = User ? User : Cast<AFirstPersonCharacter>(GetOwningActor());
    EquipState = EEquipState::Equipping;
	
	/*unhide mesh*/
	if (bAutoUnhideOnEquip && GetOwner()->IsHidden())
		GetOwner()->SetActorHiddenInGame(false);

	if (OnEquipBegin.IsBound())
		OnEquipBegin.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());

	if (IInventoryItemInterface* ItemInterface = Cast<IInventoryItemInterface>(GetOwner()))
		ItemInterface->StartEquipping(EquipRequestUser.Get(), this, GetInventoryManager());
    if (bCompleteEquipAutomatically && EquipState == EEquipState::Equipping) EndEquip();
}

void UInventoryItemComponent::EndEquip()
{
	if (EquipState != EEquipState::Equipping)
		return;
	EquipState = EEquipState::Equipped;
	if (IInventoryItemInterface* ItemInterface = Cast<IInventoryItemInterface>(GetOwner()))
		ItemInterface->StopEquipping(EquipRequestUser.Get(), this, GetInventoryManager(), EInventoryItemOperationEndReason::Completed);

    EquipRequestUser.Reset();
	if (InventoryManager)
		InventoryManager->OnEquipFinished(Cast<AInventoryItem>(GetOwner()));

	if (OnEquipEnd.IsBound())
		OnEquipEnd.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());
}

void UInventoryItemComponent::BeginUnequip()
{
	EquipState = EEquipState::Unequipping;

	if (OnUnequipBegin.IsBound())
		OnUnequipBegin.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());

	if (IInventoryItemInterface* ItemInterface = Cast<IInventoryItemInterface>(GetOwner()))
		ItemInterface->StartUnequipping(Cast<AFirstPersonCharacter>(GetOwningActor()), this, GetInventoryManager());
}

/*should be called by actor to complete equipping*/
void UInventoryItemComponent::EndUnequip()
{
	EquipState = EEquipState::NONE;
	if (IInventoryItemInterface* ItemInterface = Cast<IInventoryItemInterface>(GetOwner()))
		ItemInterface->StopUnequipping(Cast<AFirstPersonCharacter>(GetOwningActor()), this, GetInventoryManager(), EInventoryItemOperationEndReason::Completed);

	if (InventoryManager)
		InventoryManager->OnUnequipFinished(Cast<AInventoryItem>(GetOwner()));


	if (OnUnequipEnd.IsBound())
		OnUnequipEnd.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());

}

//================================================
//====================STORAGE=====================
//================================================

//
//bool UInventoryItemComponent::CanBePickedUp()
//{
//	if (GetItemPresence() != EItemPresence::World)
//		return false;	
//
//	/*return false otherwise*/
//	return true;
//}
//
//
//void UInventoryItemComponent::OnDropped()
//{
//	/*notify Loot Subsystem where we are*/
//	RegisterGridLocation();
//}

//void UInventoryItemComponent::NativeOnStoredInsideInventoryManager(UInventoryManagerComponent* InInventoryManager)
//{	
//	SetItemPresence(EItemPresence::Storage);
//
//
//}
AActor* UInventoryItemComponent::GetOwningActor()
{
	if (GetInventoryManager())
	{
		return GetInventoryManager()->GetOwner();
	}

	return nullptr;
}

void UInventoryItemComponent::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

void UInventoryItemComponent::RegisterInventoryItem()
{
	if (AInventoryItem* OwningItem = GetOwner<AInventoryItem>())
	{
		/*notify the Loot Subsystem so that it can track us*/
		if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
		{
			ISS->RegisterInventoryItem(OwningItem);
		}
	}
}

void UInventoryItemComponent::RegisterGridLocation()
{

}

void UInventoryItemComponent::UnregisterGridLocation()
{
	/*notify the Loot Subsystem that wit should no longer track our placement*/
	if (ULootSubsystem* LootSubsystem = GetWorld()->GetSubsystem<ULootSubsystem>())
	{
		LootSubsystem->UnregisterDroppedItem(this, RegisteredGridKey);
	}
}

void UInventoryItemComponent::EnableCollision()
{
	if (GetOwner())
	{
		GetOwner()->SetActorEnableCollision(true);
	}

	if (GetNetMode() < NM_Client)
		bCollisionEnabled = true;
}

void UInventoryItemComponent::DisableCollision()
{
	if (GetOwner())
	{
		GetOwner()->SetActorEnableCollision(false);
	}

	if (GetNetMode() < NM_Client)
		bCollisionEnabled = false;
}

void UInventoryItemComponent::OnRep_CollisionChanged()
{
	if (bCollisionEnabled)
		EnableCollision();
	else
		DisableCollision();
}

void UInventoryItemComponent::EnableVisibility()
{
	if (GetOwner())
	{
		GetOwner()->SetActorHiddenInGame(false);
	}
}

void UInventoryItemComponent::DisableVisibility()
{
	if (GetOwner())
	{
		GetOwner()->SetActorHiddenInGame(true);
	}
}

