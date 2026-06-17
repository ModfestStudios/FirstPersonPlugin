// Copyright (c) 2022 Pocket Sized Animations


#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Inventory/InventoryItem.h"
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

bool UInventoryItemComponent::IsSellable()
{
	return bSellableItem;
}

//====================================
//===============STATES===============
//====================================

EItemPresence UInventoryItemComponent::GetItemPresence()
{
	return ItemPresence;
}


void UInventoryItemComponent::SetItemPresence(EItemPresence NewPresence)
{
	ItemPresence = NewPresence;
}


//=================================
//============EQUIPPING============
//=================================



void UInventoryItemComponent::SetItemState(EItemState NewState)
{
	/*no need to update if already in this state*/
	if (NewState == ItemState)
		return;

	PreviousItemState = ItemState;
	ItemState = NewState;
}



/*BeginEquip() - Called by the InventoryManager as a generic entry for any Inventory Item to provide an easy universal way across different Actor-types to receive
* equipping notifications without needing to extend from a base case
* Will call the Interface-based "OnEquip" on the owning actor so they can execute their own code
* It's ASSUMED the Actor will then notify this ItemComponent it's completed via EndEquip() so it can notify the InventoryManager and update the Items State
*/
void UInventoryItemComponent::BeginEquip()
{
	EquipState = EEquipState::Equipping;
	
	/*unhide mesh*/
	if (bAutoUnhideOnEquip && GetOwner()->IsHidden())
		GetOwner()->SetActorHiddenInGame(false);

		/*notify owner (if using interface) of Equip*/
	if (GetOwner()->Implements<UInventoryItemInterface>())
		IInventoryItemInterface::Execute_OnEquip(GetOwner());

	if (OnEquipBegin.IsBound())
		OnEquipBegin.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());
}


void UInventoryItemComponent::EndEquip()
{
	EquipState = EEquipState::Equipped;
	ItemState = EItemState::InUse;
	InventoryManager->OnEquipFinished(Cast<AInventoryItem>(GetOwner()));

	if (OnEquipEnd.IsBound())
		OnEquipEnd.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());
}

void UInventoryItemComponent::BeginUnequip()
{
	EquipState = EEquipState::Unequipping;

	/*easy hook using interfaces*/
	if (GetOwner()->Implements<UInventoryItemInterface>())
		IInventoryItemInterface::Execute_OnUnequip(GetOwner());

	if (OnUnequipBegin.IsBound())
		OnUnequipBegin.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());
}


/*should be called by actor to complete equipping*/
void UInventoryItemComponent::EndUnequip()
{
	EquipState = EEquipState::NONE;
	InventoryManager->OnUnequipFinished(Cast<AInventoryItem>(GetOwner()));


	if (OnUnequipEnd.IsBound())
		OnUnequipEnd.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());

}
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
//void UInventoryItemComponent::PickedUp(AActor* Instigator, UInventoryManagerComponent* InstigatorInvManager)
//{
//	if (GetOwner()->Implements<UInventoryItemInterface>())
//		IInventoryItemInterface::Execute_OnPickedUp(GetOwner());
//
//	/*notify loot subsystem we've been picked up*/
//	UnregisterGridLocation();
//
//	/*send notification*/
//	if (OnPickedUp.IsBound())
//		OnPickedUp.Broadcast(GetOwner(), this, GetInventoryManager(), GetOwner()->GetOwner());
//}
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

