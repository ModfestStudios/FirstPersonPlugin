// Copyright (c) 2022 Pocket Sized Animations

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"
#include "Inventory/InventoryEquipmentSlot.h"
#include "Subsystems/LootSubsystem.h"
#include "Inventory/InventoryLoadout.h"
#include "Inventory/InventoryItemSlot.h"

/*logging*/
#include "../Logging.h"

/*replication*/
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"


/*players*/
#include "Players/FirstPersonPlayerController.h"
#include "Characters/FirstPersonCharacter.h"

/*subsystems*/
#include "Subsystems/InventorySubsystem.h"

/*ui*/
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/InventoryWidget.h"

/*setup replication*/
void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagerComponent, CurrentlyEquipped);
	DOREPLIFETIME(UInventoryManagerComponent, PrimaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, SecondaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, AlternativeWeapon);
	//DOREPLIFETIME(UInventoryManagerComponent, Inventory);
	DOREPLIFETIME(UInventoryManagerComponent, InventorySlots);
}

bool UInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicationOccured = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (UInventoryItemSlot* Item : InventorySlots)
	{
		if (IsValid(Item))
			bReplicationOccured |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
	}

	return bReplicationOccured;
}

// Sets default values for this component's properties
UInventoryManagerComponent::UInventoryManagerComponent()
{
	Quickslots.Init(nullptr, 11);
	bCanEquipItems = true;
	bEnablePrimaryWeapon = true;
	bAutoEquipPrimaryOnSpawn = true;
	bEnableSecondaryWeapon = true;
	bEnableAlternativeWeapon = true;

	/*replication*/
	SetIsReplicatedByDefault(true);

	/*ticking*/
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.15f;
}


#if WITH_EDITOR
void UInventoryManagerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}

void UInventoryManagerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName MemberName = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()
		? PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName()
		: NAME_None;

	const FName PropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (MemberName == GET_MEMBER_NAME_CHECKED(UInventoryManagerComponent, DefaultItems))
	{
		const int32 Index = PropertyChangedEvent.GetArrayIndex(
			GET_MEMBER_NAME_STRING_CHECKED(UInventoryManagerComponent, DefaultItems)
		);

		if (DefaultItems.IsValidIndex(Index))
		{
			//FInventoryItemSpawnParams& Entry = DefaultItems[Index];

			if (PropertyName == GET_MEMBER_NAME_CHECKED(FInventoryItemSpawnParams, ItemClass))
			{
				DefaultItems[Index].DisplayName = DefaultItems[Index].ItemClass
					? DefaultItems[Index].ItemClass->GetName()
					: TEXT("None");
			}
		}
	}
}

#endif


// Called when the game starts
void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	/*register ourselves with the subsystem*/
	RegisterInventoryManager();

	/*spawn the default inventory*/
	if (InventorySlots.Num() == 0)//we check if Inventory == 0 because BeginPlay() is called multiple times, so this saves overhead
		SpawnDefaultInventory();
}



void UInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCheckForItemsInVincinityOnTick)
		UpdateInventoryItemsInVicinity();
	if (bCheckForManagersInVicinityOnTick)
		UpdateInventoryManagersInVicinity();
}


void UInventoryManagerComponent::SpawnDefaultInventory()
{
	/*server check*/
	if (GetNetMode() == NM_Client)
		return;

	if (DefaultItems.IsEmpty() == false)
		SpawnInventoryItems(DefaultItems);
}

void UInventoryManagerComponent::SpawnInventoryFromLoadout(AInventoryLoadout* Loadout, bool bClearExisting)
{
	/*servery check*/
	if (GetNetMode() == NM_Client)
		return;

	/*safety check*/
	if(!IsValid(Loadout))
		return;

	TSubclassOf<AInventoryItem> Primary = Loadout->PrimaryWeapon.ItemClass;
	//TArray<TSubclassOf<AInventoryItem>> InventoryItems = Loadout->Items;

	if (Primary)
	{
		AInventoryItem* NewPrimary = SpawnInventoryItem(Primary);
		AssignPrimaryWeapon(NewPrimary);

		if (bAutoEquipPrimaryOnSpawn)
			Equip(GetPrimaryWeapon());
	}

	//if (InventoryItems.Num() > 0)
	//	SpawnInventory(InventoryItems, false);

}

void UInventoryManagerComponent::SpawnInventoryItems(TArray<FInventoryItemSpawnParams> Inventory)
{
	/*safety checks*/
	if (!IsValid(GetOwner()) || Inventory.Num() == 0 || GetNetMode() == NM_Client)
		return;

	for (FInventoryItemSpawnParams SpawnParams : Inventory)
	{
		/*assign it an equipment slot*/
		if (SpawnParams.bEquipOnSpawn)
		{

		}
		/*otherwise put it inside an inventory container*/
		else
		{

		}
	}
}

void UInventoryManagerComponent::SpawnInventory(TArray<TSubclassOf<AInventoryItem>> Items, bool bClearExisting)
{
	/*safety/server-check*/
	if (GetOwner() == nullptr || GetOwner()->IsPendingKillPending() || GetNetMode() == NM_Client)
		return;

	/*destroy existing inventory*/
	if (bClearExisting)
		ClearInventory();

	/*auto-equip*/
	AInventoryItem* AutoEquip = nullptr;

	for (TSubclassOf<AInventoryItem> ItemClass : Items)
	{
		/*skip blank entries*/
		if (ItemClass == nullptr)
			continue;


		/*create the initial stored item state*/
		UInventoryItemSlot* NewItem = CreateInventorySlot(ItemClass);

		if (NewItem)
		{
			InventorySlots.Add(NewItem);			
		}



		/*spawn item*/
		AInventoryItem* Item = SpawnInventoryItem(ItemClass);



		if (Item)
		{
			if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
			{
				AInventoryItem* OwningItem = Cast<AInventoryItem>(GetOwner());
				AddItem(OwningItem);


				if (ShouldAutoEquip(Item))
					AutoEquip = Item;
			}
		}
	}

	if (AutoEquip != nullptr)
		Equip(AutoEquip);

	/*force net update since we have no Subobjects to replicate*/
	if (AActor* Owner = GetOwner())
		Owner->ForceNetUpdate();
}

AInventoryItem* UInventoryManagerComponent::SpawnInventoryItem(TSubclassOf<AInventoryItem> ItemClass)
{
	/*safety/server-check*/
	if (Cast<AInventoryItem>(GetOwner()) == nullptr || GetNetMode() == NM_Client)
		return nullptr;

	/*initialize*/
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = Cast<AInventoryItem>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Cast<AInventoryItem>(GetOwner())->GetActorLocation());
	SpawnTransform.SetRotation(Cast<AInventoryItem>(GetOwner())->GetActorRotation().Quaternion());
	SpawnTransform.SetScale3D(FVector(1));

	return GetWorld()->SpawnActor<AInventoryItem>(ItemClass, SpawnTransform, SpawnParams);
}

void UInventoryManagerComponent::ClearInventory()
{

}

AInventoryItem* UInventoryManagerComponent::GetCurrentlyEquippedItem()
{
	return CurrentlyEquipped;
}



//======================================
//===============VICINITY===============
//======================================


void UInventoryManagerComponent::UpdateInventoryItemsInVicinity()
{
	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{		
		const FVector OwnerLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector(0);		
		TArray<AInventoryItem*> NewItemList = ISS->GetInventoryItemsInVicinity(OwnerLocation, MaxVicinityItemDistance);
				
		/*make sure there is a difference*/
		if (!ISS->AreInventoryItemListsIdentical(ItemsInVicinity, NewItemList))
		{
			ItemsInVicinity = NewItemList;

			if (OnInventoryItemsInVicinityUpdated.IsBound())
				OnInventoryItemsInVicinityUpdated.Broadcast();
		}
	}
}

TArray<AInventoryItem*> UInventoryManagerComponent::GetInventoryItemsInVicinity()
{
	return ItemsInVicinity;
}

void UInventoryManagerComponent::UpdateInventoryManagersInVicinity()
{
	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{		
		FVector OwnerLocation = GetOwner()->GetActorLocation();
		FInventoryManagerVicinityCheckParameters VicinityCheckParams;
		VicinityCheckParams.IgnoredActors.Add(GetOwner());
		TArray<UInventoryManagerComponent*> NewManagerList = ISS->GetInventoryManagersNearby(OwnerLocation, MaxVicinityItemDistance, VicinityCheckParams);
		
		/*check for differences*/
		if (!ISS->AreInventoryManagerListsIdentical(ManagersInVicinity, NewManagerList))
		{
			ManagersInVicinity = NewManagerList;

			if (OnInventoryManagersInVicinityUpdated.IsBound())
				OnInventoryManagersInVicinityUpdated.Broadcast();
		}
	}
}

TArray< UInventoryManagerComponent*> UInventoryManagerComponent::GetInventoryManagersInVicinity()
{
	return ManagersInVicinity;
}





//=====================================
//==============EQUIPPING==============
//=====================================

void UInventoryManagerComponent::Equip(AInventoryItem* Item)
{
	bool bInstantComplete = false;

	/*validation check*/
	if (!bCanEquipItems || Item == nullptr)
	{
#if WITH_EDITOR
		if (bCanEquipItems == false)
		{
			UE_LOG(LogInventoryModule, Warning, TEXT("!!!%s::Equip() called but InventoryManager has bCanEquipItems set false!!!"), *GetName());
		}
		else if (Item == nullptr)
		{
			UE_LOG(LogInventoryModule, Warning, TEXT("!!!%s::Equip() called but item is null!"), *GetName());
		}
#endif

		return;
	}

	/*ensure-registration is complete*/
	RegisterItem(Item);

	/*if we already have the item equipped - go ahead and auto-unequip it*/
	if (Item == CurrentlyEquipped)
	{
		Unequip(CurrentlyEquipped);
		return;
	}

	/*unequip anything we already have first*/
	if (CurrentlyEquipped != nullptr)
	{
		PendingEquip = Item;
		Unequip(Item);
		return;
	}

	/*mark this item as pending equip*/
	PendingEquip = Item;

	/*notify the component it's being equipped */
	if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
	{
		ItemComp->BeginEquip();
	}
}


void UInventoryManagerComponent::EquipToEquipmentSlot(AInventoryItem* Item, UInventoryEquipmentSlot* EquipmentSlot)
{
	if (!Item || !EquipmentSlot)
		return;

	EquipmentSlot->InventoryItem = Item;
}

/*callback function - should be triggered by the item itself*/
void UInventoryManagerComponent::OnEquipFinished(AInventoryItem* Item)
{
	if (Item == nullptr)
		return;

	CurrentlyEquipped = Item;
	PendingEquip = nullptr;

	/*event/delegate notifications*/
	if (CurrentlyEquipped != nullptr && OnItemEquipped.IsBound()) // any item
		OnItemEquipped.Broadcast(CurrentlyEquipped);
	if (CurrentlyEquipped != nullptr && CurrentlyEquipped == PrimaryWeapon && OnPrimaryEquipped.IsBound()) //primary weapon
		OnPrimaryEquipped.Broadcast(CurrentlyEquipped);
	if (CurrentlyEquipped != nullptr && CurrentlyEquipped == SecondaryWeapon && OnSecondaryEquipped.IsBound()) //secondary weapon
		OnSecondaryEquipped.Broadcast(CurrentlyEquipped);
	if (CurrentlyEquipped != nullptr && CurrentlyEquipped == AlternativeWeapon && OnAlternativeEquipped.IsBound()) //alternative wepaon
		OnAlternativeEquipped.Broadcast(CurrentlyEquipped);
}

void UInventoryManagerComponent::Unequip(AInventoryItem* Item)
{
	/*safetry check*/
	if (Item == nullptr)
		return;

	if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
	{
		ItemComp->BeginUnequip();
	}
}

void UInventoryManagerComponent::OnUnequipFinished(AInventoryItem* Item)
{
	CurrentlyEquipped = nullptr;

	/*if we're waiting to equip anything - go ahead*/
	if (PendingEquip)
	{
		Equip(PendingEquip);
	}
}

bool UInventoryManagerComponent::ShouldAutoEquip(AInventoryItem* Item, bool bItemPickedUp)
{
	if (!Item)
		return false;

	if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
	{
		if (ItemComp->bAutoEquip)
			return true;
	}

	return false;
}

//============================================
//=================QUICKSLOTS=================
//============================================


AInventoryItem* UInventoryManagerComponent::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

void UInventoryManagerComponent::AssignPrimaryWeapon(AInventoryItem* Weapon)
{
	if (!Weapon)
		return;

	if (bCanEquipItems && bEnablePrimaryWeapon)
		PrimaryWeapon = Weapon;

	if (OnPrimaryAssigned.IsBound())
		OnPrimaryAssigned.Broadcast(this, Cast<AInventoryItem>(GetOwner()), PrimaryWeapon);
}

void UInventoryManagerComponent::EquipPrimaryWeapon()
{
	if (bCanEquipItems && bEnablePrimaryWeapon && PrimaryWeapon)
		Equip(PrimaryWeapon);
}

AInventoryItem* UInventoryManagerComponent::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void UInventoryManagerComponent::AssignSecondaryWeapon(AInventoryItem* Weapon)
{
	if (!Weapon)
		return;

	if (bCanEquipItems && bEnableSecondaryWeapon)
		SecondaryWeapon = Weapon;


	if (OnSecondaryAssigned.IsBound())
		OnSecondaryAssigned.Broadcast(this, Cast<AInventoryItem>(GetOwner()), SecondaryWeapon);
}

void UInventoryManagerComponent::EquipSecondaryWeapon()
{
	if (bCanEquipItems && bEnableSecondaryWeapon && SecondaryWeapon)
		Equip(SecondaryWeapon);
}

AInventoryItem* UInventoryManagerComponent::GetAlternativeWeapon()
{
	return AlternativeWeapon;
}

void UInventoryManagerComponent::AssignAlternativeWeapon(AInventoryItem* Weapon)
{
	if (!Weapon)
		return;

	if (bCanEquipItems && bEnableAlternativeWeapon)
		AlternativeWeapon = Weapon;


	if (OnAlternativeAssigned.IsBound())
		OnAlternativeAssigned.Broadcast(this, Cast<AInventoryItem>(GetOwner()), AlternativeWeapon);
}


void UInventoryManagerComponent::EquipAlternativeWeapon()
{
	if (bCanEquipItems && bEnableAlternativeWeapon && AlternativeWeapon)
		Equip(AlternativeWeapon);
}



void UInventoryManagerComponent::AssignToQuickslot(AInventoryItem* ActorToAssign, EQuickslot Quickslot)
{
	/*safety check*/
	if (ActorToAssign == nullptr)
		return;

	Quickslots[(uint8)Quickslot] = ActorToAssign;
}

void UInventoryManagerComponent::EquipQuickslot(EQuickslot Quickslot)
{
	if (Quickslots[(uint8)Quickslot] != nullptr)
		Equip(Quickslots[(uint8)Quickslot]);
}

AInventoryItem* UInventoryManagerComponent::GetQuickslotItem(EQuickslot Quickslot)
{
	return Quickslots[(uint8)Quickslot];
}



//const TArray<AInventoryItem*> UInventoryManagerComponent::GetInventory() const
//{
//	return Inventory;
//}

const TArray<class UInventoryItemSlot*> UInventoryManagerComponent::GetInventorySlots() const
{
	return InventorySlots;
}

const TArray<class UInventoryItemSlot*> UInventoryManagerComponent::GetInventorySlotsOfSize(EItemSize ItemSize)
{
	TArray<UInventoryItemSlot*> FilteredItems;

	for (UInventoryItemSlot* Item : InventorySlots)
	{
		if (!Item)
			continue;

		if (UInventoryItemComponent* ItemComp = Item->ItemClass->GetDefaultObject<AInventoryItem>()->GetItemComponent())
		{
			/*check size*/
			if (ItemComp->Size == ItemSize)
				FilteredItems.Add(Item);
		}
	}

	return FilteredItems;
}

//=======================================
//============ITEM MANAGEMENT============
//=======================================

bool UInventoryManagerComponent::ContainsItem(AInventoryItem* Item)
{
	/*safety check*/
	if (!Item)
		return false;

	/*look through items and check to see if this item exists*/
	for (UInventoryItemSlot* ItemSlot : InventorySlots)
	{
		if (!IsValid(ItemSlot))
			continue;

		if (ItemSlot->InventoryItem == Item)
			return true;
	}
	
	return false;
}

bool UInventoryManagerComponent::AddItem(AInventoryItem* Item)
{
	if (!Item)
		return false;


	UInventoryItemSlot* ItemSlot = NewObject<UInventoryItemSlot>();
	
	if (ItemSlot)
	{
		ItemSlot->InventoryItem = Item;
		ItemSlot->ItemClass = Item->GetClass();
		
		InventorySlots.Add(ItemSlot);

		RegisterItem(Item);
		StoreItem(Item);
		NativeOnInventoryUpdated();

		/*if (OnItemAdded.IsBound())
			OnItemAdded.Broadcast(this, Cast<AInventoryItem>(GetOwner()), Item);*/

		return true;
	}

	else
		return false;
}

bool UInventoryManagerComponent::RemoveItem(AInventoryItem* Item)
{
	if (!Item)
		return false;

	Item->SetOwner(nullptr);
	NativeOnInventoryUpdated();

	return false;
}

void UInventoryManagerComponent::PickupItem(AInventoryItem* Item)
{
	/*safety check*/
	if (!Item)
		return;

	AddItem(Item);

	/*automatically equip the item if that seems appropriate for our lives*/
	if (ShouldAutoEquip(Item, true))
		Equip(Item);


	/*broadcast we've picked the item up*/
	if (OnItemPickedUp.IsBound())
		OnItemPickedUp.Broadcast(this, Cast<AInventoryItem>(GetOwner()), Item);

	/*notify item it's been picked up*/
	Item->OnItemPickedUp(GetOwner());

	NativeOnInventoryUpdated();
}

bool UInventoryManagerComponent::CanPickup(AInventoryItem* Item)
{
	if (!Item)
		return false;

	return Item->CanBePickedUp();
}

void UInventoryManagerComponent::DropItem(AInventoryItem* Item)
{
	/*safety check*/
	if (!Item)
		return;

	Item->OnItemDropped(GetOwner());

	/*make sure we remove it from us*/
	RemoveItem(Item);

	/*trigger event*/
	NativeOnInventoryUpdated();
}



void UInventoryManagerComponent::StoreItem(AInventoryItem* Item)
{
	if (UInventoryItemComponent* ItemComp = Cast<UInventoryItemComponent>(Item->GetComponentByClass(UInventoryItemComponent::StaticClass())))
	{
		/*safety check*/
		if (ContainsItem(Item) == false)
		{
			AddItem(Item);
		}

		ItemComp->SetItemState(EItemState::InStorage);
		ItemComp->DisableCollision();

		NativeOnInventoryUpdated();
		//ItemComp->DisableVisibility();
	}
}

void UInventoryManagerComponent::RegisterItem(AInventoryItem* Item)
{
	if (!Item)
		return;

	if (UInventoryItemComponent* ItemComp = Cast<UInventoryItemComponent>(Item->GetComponentByClass(UInventoryItemComponent::StaticClass())))
	{
		Item->SetOwner(Cast<AInventoryItem>(GetOwner()));
		ItemComp->InventoryManager = this;
	}
}

//===================
//=======SLOTS=======
//===================

void UInventoryManagerComponent::MoveInventorySlotToInventory(UInventoryItemSlot* InventorySlot)
{
	/*safety checks*/
	if (!InventorySlot || InventorySlots.Contains(InventorySlot) || !CanInventorySlotFitInInventory(InventorySlot))
		return;


	
	bool bRemoveFromPreviousManager = IsValid(InventorySlot->GetOwner());
	class UInventoryManagerComponent* PreviousManager = IsValid(InventorySlot->GetOwner()) ? InventorySlot->GetOwner() : nullptr;
	

	AddInventorySlotToInventory(InventorySlot);
	
	/*make sure we remove it from the previous owner*/
	if (bRemoveFromPreviousManager)
		PreviousManager->RemoveInventorySlotFromInventory(InventorySlot);
	
}

bool UInventoryManagerComponent::CanInventorySlotFitInInventory(UInventoryItemSlot* InventorySlot)
{
	if (!InventorySlot)
		return false;

	if (InventorySlot->GetItemSlotSize() > GetRemainingSlotCapacity())
		return false;

	return true;
}

void UInventoryManagerComponent::AddInventorySlotToInventory(UInventoryItemSlot* InventorySlot)
{
	if (!InventorySlot || !GetOwner()->HasAuthority())
		return;

	/*initialize*/
	AInventoryItem* Item = InventorySlot->InventoryItem.IsValid() ? InventorySlot->InventoryItem.Get() : nullptr;

	
	int32 AddedIndex = InventorySlots.Add(InventorySlot);
	InventorySlot->SetOwner(this);
	InventorySlot->OnAddedToInventoryManager(this);
	
	/*if an instance exists*/
	if (Item)
	{		
		Item->OnItemStored(GetOwner());
	}
	
	NativeOnInventoryUpdated();
}

void UInventoryManagerComponent::RemoveInventorySlotFromInventory(UInventoryItemSlot* InventorySlot)
{
	if (!InventorySlot || !GetOwner() || !GetOwner()->HasAuthority())
		return;

	
	InventorySlots.Remove(InventorySlot);

	/*unregister the inventory manager from this item slot*/
	if (InventorySlot->GetOwner() == this)
		InventorySlot->SetOwner(nullptr);

	InventorySlot->OnRemovedFromInventoryManager(this);

	NativeOnInventoryUpdated();	
}

int32 UInventoryManagerComponent::GetMaxSlotCapacity() const
{
	return SlotCapacity;
}

int32 UInventoryManagerComponent::GetRemainingSlotCapacity() const
{	
	return FMath::Max(0, SlotCapacity - GetUsedSlotCapacity());
}

int32 UInventoryManagerComponent::GetUsedSlotCapacity() const
{
	int32 UsedSlots = 0;
	
	for (UInventoryItemSlot* ItemSlot : InventorySlots)
	{
		const int32 ItemSize = GetItemSlotSize(ItemSlot->GetItemSize());

		UsedSlots += ItemSize;
	}

	return UsedSlots;
}


int32 UInventoryManagerComponent::GetMaxItemCountForSize(EItemSize Size) const
{
	const int32 InventoryCapacity = SlotCapacity;
	const int32 ItemSlotSize = GetItemSlotSize(Size);
	const int32 CapacityForSize = InventoryCapacity / ItemSlotSize;


	return CapacityForSize;
}

int32 UInventoryManagerComponent::GetRemainingItemCountForSize(EItemSize Size) const
{
	const int32 RemainingSlots = GetRemainingSlotCapacity();
	const int32 ItemSlotSize = GetItemSlotSize(Size);

	if (ItemSlotSize == 0)
	{
		return 0;
	}

	return RemainingSlots / ItemSlotSize;
}

int32 UInventoryManagerComponent::GetUsedItemCountForSize(EItemSize Size) const
{	
	int32 ConsumedCapacity = 0;

	for (UInventoryItemSlot* ItemSlot : InventorySlots)
	{
		if (!IsValid(ItemSlot))
			continue;

		if (ItemSlot->GetItemSize() == Size)
		{
			ConsumedCapacity += 1;
		}
	}
	
	return ConsumedCapacity;
}

int32 UInventoryManagerComponent::GetItemSlotSize(EItemSize Size) const
{
	switch (Size)
	{
		case EItemSize::Small:
			return INVENTORY_SLOT_SIZE_SM;			
		case EItemSize::Medium:
			return INVENTORY_SLOT_SIZE_MD;			
		case EItemSize::Large:
			return INVENTORY_SLOT_SIZE_LG;			
		case EItemSize::ExtraLarge:
			return INVENTORY_SLOT_SIZE_XLG;			
		default:
			return INVENTORY_SLOT_SIZE_SM;			
	}
}

UInventoryEquipmentSlot* UInventoryManagerComponent::GetEquipmentSlot(FName ID)
{
	for (UInventoryEquipmentSlot* Slot : EquipmentSlots)
	{
		if (Slot->SlotID == ID)
			return Slot;
	}

	return nullptr;
}


/* CreateInventoryItemSlot() - Creates an Inventory Item Slot
*
*
*
*
*/
UInventoryItemSlot* UInventoryManagerComponent::CreateInventoryItemSlot(FInventoryItemSpawnParams ItemSpawnParams, bool bSpawnInstance)
{
	TSubclassOf<AInventoryItem> ItemClass = ItemSpawnParams.ItemClass;
	bool bEquipOnSpawn = ItemSpawnParams.bEquipOnSpawn;

	if (GetNetMode() == NM_Client || ItemClass == nullptr)
		return nullptr;

	UInventoryItemSlot* ItemSlot = NewObject<UInventoryItemSlot>(this);

	ItemSlot->ItemClass = ItemClass;

	if (bSpawnInstance || bEquipOnSpawn) //we have to spawn to auto-equip
	{
		FVector SpawnLocation = GetOwner()->GetActorLocation();
		FRotator SpawnRotation = GetOwner()->GetActorRotation();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = GetOwner();
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AInventoryItem* ItemInstance = GetWorld()->SpawnActor<AInventoryItem>(ItemClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

		if (ItemInstance)
		{
			ItemSlot->InventoryItem = ItemInstance;

			ItemInstance->SetActorEnableCollision(false);

			if (bEquipOnSpawn)
			{
				if (UInventoryEquipmentSlot* EquipmentSlot = GetEquipmentSlotByClass(ItemClass))
				{
					EquipToEquipmentSlot(ItemInstance, EquipmentSlot);
				}
			}
		}		
	}

	return ItemSlot;
}

UInventoryItemSlot* UInventoryManagerComponent::CreateInventorySlot(TSubclassOf<class AInventoryItem> ItemClass)
{
	/*safety check*/
	if (GetNetMode() == NM_Client)
		return nullptr;

	UInventoryItemSlot* NewItem = NewObject<UInventoryItemSlot>(this);
	NewItem->ItemClass = ItemClass;

	return NewItem;
}

UInventoryEquipmentSlot* UInventoryManagerComponent::GetEquipmentSlotByClass(TSubclassOf<AInventoryItem> ItemClass)
{
	for (UInventoryEquipmentSlot* Slot : EquipmentSlots)
	{
		if (Slot && Slot->ItemType->IsChildOf(ItemClass))
			return Slot;
	}

	return nullptr;
}


UInventoryEquipmentSlot* UInventoryManagerComponent::CreateDefaultEquipmentSlot(FName SubobjectName, TSubclassOf<class AInventoryItem> ItemType)
{
	UInventoryEquipmentSlot* Slot = CreateDefaultSubobject<UInventoryEquipmentSlot>(SubobjectName);

	Slot->ItemType = ItemType;
	Slot->SlotID = SubobjectName;

	return Slot;
}


void UInventoryManagerComponent::NativeOnInventoryUpdated()
{
	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->NativeOnSelectedInventoryManagerUpdated(this); //notify the Inventory Subsystem so it can do its thing as needed
	}

	/*notify anyone listening*/
	if (OnInventoryUpdated.IsBound())
		OnInventoryUpdated.Broadcast();

}


//==============================
//=============GRID=============
//==============================


void UInventoryManagerComponent::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

UInventoryItemComponent* UInventoryManagerComponent::GetItemComponent(AInventoryItem* Item)
{
	if (!Item)
		return nullptr;

	return Cast<UInventoryItemComponent>(Item->GetComponentByClass(UInventoryItemComponent::StaticClass()));
}



void UInventoryManagerComponent::InitializeInventoryWidget(APlayerController* Controller)
{
	if (InventoryWidgetClass == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s::InitializeInventoryWidget() - !!! InventoryWidgetClass Missing! Please set class inside the Inventory Manager Component !!!"), *GetNameSafe());
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(PC, InventoryWidgetClass,"InventoryWidget");

	/*	if (InventoryWidget)
			InventoryWidget->InitializeInventoryWidget(this);*/
	}
}

FText UInventoryManagerComponent::GetInventoryManagerName()
{
	return InventoryManagerName;
}

UMaterialInterface* UInventoryManagerComponent::GetInventoryIcon()
{
	return PrestineInventoryIcon;
}

//==============================
//==============UI==============
//==============================

void UInventoryManagerComponent::OpenInventoryWidget()
{
	if (InventoryWidget)
	{
		InventoryWidget->AddToPlayerScreen();
		InventoryWidget->SetFocus();

		if (AFirstPersonCharacter* Character = GetOwner<AFirstPersonCharacter>())
		{
			if (AFirstPersonPlayerController* PC = Character->GetController<AFirstPersonPlayerController>())
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly::FInputModeUIOnly());
			}
		}

		UpdateInventoryManagersInVicinity();
		UpdateInventoryItemsInVicinity();

		if (OnInventoryManagerOpened.IsBound())
			OnInventoryManagerOpened.Broadcast();
	}
}

void UInventoryManagerComponent::CloseInventoryWidget()
{
	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();		

		if (AFirstPersonCharacter* Character = GetOwner<AFirstPersonCharacter>())
		{
			if (AFirstPersonPlayerController* PC = Character->GetController<AFirstPersonPlayerController>())
			{
				PC->bShowMouseCursor = false;
				PC->SetInputMode(FInputModeGameOnly::FInputModeGameOnly());
			}
		}

		if (OnInventoryManagerClosed.IsBound())
			OnInventoryManagerClosed.Broadcast();
	}

	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->ClearSelectedInventoryManager();
		ISS->ClearHoveredInventoryManager();
	}

}

bool UInventoryManagerComponent::IsInventoryWidgetOpen()
{
	if (!InventoryWidget)
		return false;	

	return InventoryWidget->IsInViewport();
}



//==================================
//===========REGISTRATION===========
//==================================

/*register ourselves with the Inventory Subsystem so they know we exist*/
void UInventoryManagerComponent::RegisterInventoryManager()
{
	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->RegisterInventoryManager(this);
	}
}

