// Copyright (c) 2022 Pocket Sized Animations

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "UObject/GarbageCollection.h"
#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Components/PrimitiveComponent.h"
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

	DOREPLIFETIME(UInventoryManagerComponent, EquippedItem);
	DOREPLIFETIME(UInventoryManagerComponent, PrimaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, SecondaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, AlternativeWeapon);
	//DOREPLIFETIME(UInventoryManagerComponent, Inventory);
	DOREPLIFETIME(UInventoryManagerComponent, InventorySlots);
	DOREPLIFETIME(UInventoryManagerComponent, EquipmentSlots);
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
	EquipmentSlotComponentClass = UInventoryAttachmentComponent::StaticClass();
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
#endif

#if WITH_EDITOR
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
		const int32 Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UInventoryManagerComponent, DefaultItems));

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
    RegisterInventoryManager();
    if (GetOwner()->HasAuthority())
    {
        InitializeEquipmentSlots();
        SpawnDefaultInventory();
    }
}

void UInventoryManagerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    const bool bCanAccessWorld = !IsGarbageCollecting() && !HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) && IsValid(World) && !World->bIsTearingDown;

    // Gameplay transfers are only appropriate when removing a live component, not during PIE/GC teardown.
    if (bCanAccessWorld && !bDestroyingHierarchy && IsValid(Owner) && !Owner->IsActorBeingDestroyed() && Owner->HasAuthority())
    {
        ClearInventory();
        for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
            if (IsValid(Slot) && Slot->GetAttachedItem())
                DropItem(Slot->GetAttachedItem());
    }

    if (!IsGarbageCollecting())
        for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
            if (IsValid(Slot))
                Slot->DestroyComponent();
    EquipmentSlots.Empty();

    if (bCanAccessWorld)
        if (UInventorySubsystem* Subsystem = World->GetSubsystem<UInventorySubsystem>())
            Subsystem->UnregisterInventoryManager(this);

    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	if (AFirstPersonCharacter* OwningChar = GetOwner<AFirstPersonCharacter>())
	{
		if (OwningChar->IsLocallyControlled() || GetNetMode() < NM_Client)
		{
			if (bCheckForItemsInVicinityOnTick)
				UpdateInventoryItemsInVicinity();
			if (bCheckForManagersInVicinityOnTick)
				UpdateInventoryManagersInVicinity();
		}
	}
}

void UInventoryManagerComponent::OnOwnerPossessed(AController* NewController)
{
	if (!NewController)
		return;

	if (NewController->IsLocalPlayerController())
	{
		UInventoryManagerComponent* DefaultInventoryManager = GetOwner()->GetClass()->GetDefaultObject<AActor>()->GetComponentByClass<UInventoryManagerComponent>();
		bool bDefaultItemCheck = DefaultInventoryManager ? DefaultInventoryManager->bCheckForItemsInVicinityOnTick : false;
		bool bDefaultManagerCheck = DefaultInventoryManager ? DefaultInventoryManager->bCheckForManagersInVicinityOnTick : false;
		
		bCheckForItemsInVicinityOnTick = bDefaultItemCheck;
		bCheckForManagersInVicinityOnTick = bDefaultManagerCheck;
	}
}

void UInventoryManagerComponent::OnOwnerUnPossessed()
{	
	if (AFirstPersonCharacter* OwningChar = GetOwner<AFirstPersonCharacter>())
	{
		if (OwningChar->IsLocallyControlled())
		{
			bCheckForItemsInVicinityOnTick = false;
			bCheckForManagersInVicinityOnTick = false;
		}
	}
}

//================================================
//===================INVENTORY====================
//================================================

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
    if (!IsValid(Loadout) || !GetOwner() || !GetOwner()->HasAuthority())
        return;
    if (bClearExisting) ClearInventory();

    TArray<FInventoryItemSpawnParams> Entries = Loadout->Items;

    for (FInventoryItemSpawnParams Entry : {Loadout->PrimaryWeapon, Loadout->SecondaryWeapon, Loadout->AlternativeWeapon})
        if (Entry.ItemClass)
        {
            Entry.bEquipOnSpawn = true;
            Entries.Add(Entry);
        }
    SpawnInventoryItems(Entries);
}

void UInventoryManagerComponent::SpawnInventoryItems(TArray<FInventoryItemSpawnParams> Inventory)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
    // Equipped containers must exist before ordinary default items need storage.
    const auto SortDefaultItems = [](const FInventoryItemSpawnParams& A, const FInventoryItemSpawnParams& B)
    {
        auto AutoEquip = [](const FInventoryItemSpawnParams& Entry)
        {
            const AInventoryItem* Default = Entry.ItemClass ? Entry.ItemClass->GetDefaultObject<AInventoryItem>() : nullptr;
            const UInventoryItemComponent* Item = Default ? Default->FindComponentByClass<UInventoryItemComponent>() : nullptr;

            return Entry.bEquipOnSpawn || (Item && Item->bAutoEquip);
        };
        auto Rank = [&AutoEquip](const FInventoryItemSpawnParams& Entry)
        {
            if (!AutoEquip(Entry))
                return 2;

            const AInventoryItem* Default = Entry.ItemClass ? Entry.ItemClass->GetDefaultObject<AInventoryItem>() : nullptr;
            const UInventoryManagerComponent* Container = Default ? Default->FindComponentByClass<UInventoryManagerComponent>() : nullptr;

            return Container && (Container->SlotCapacity > 0 || !Container->EquipmentSlotConfigs.IsEmpty()) ? 0 : 1;
        };
        return Rank(A) < Rank(B);
    };

    Inventory.StableSort(SortDefaultItems);
    for (const FInventoryItemSpawnParams& Params : Inventory)
        for (int32 Index = 0; Index < Params.Quantity; ++Index)
        {
            AInventoryItem* Item = SpawnInventoryItem(Params.ItemClass);

            if (!Item) continue;

            UInventoryItemComponent* Component = GetItemComponent(Item);

            if (Component) Component->Condition = Params.Condition;

            const bool bAutoEquip = Params.bEquipOnSpawn || (Component && Component->bAutoEquip);

            if (!(bAutoEquip && TryEquipItem(Item)) && !AddItem(Item))
                UE_LOG(LogInventoryModule, Warning, TEXT("%s: default item %s could not fit; left in the world."), *GetNameSafe(GetOwner()), *GetNameSafe(Item));
        }
}

void UInventoryManagerComponent::SpawnInventory(TArray<TSubclassOf<AInventoryItem>> Items, bool bClearExisting)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
    if (bClearExisting) ClearInventory();

    TArray<FInventoryItemSpawnParams> Entries;

    for (TSubclassOf<AInventoryItem> ItemClass : Items)
    {
        FInventoryItemSpawnParams Entry;

        Entry.ItemClass = ItemClass;
        Entries.Add(Entry);
    }
    SpawnInventoryItems(Entries);
}

AInventoryItem* UInventoryManagerComponent::SpawnInventoryItem(TSubclassOf<AInventoryItem> ItemClass)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !ItemClass)
        return nullptr;
    // Prevent recursive default loadouts from spawning endlessly.
    int32 ContainerDepth = 0;
    TSet<AActor*> Parents;

    for (AActor* Parent = GetOwner(); Parent; Parent = Parent->GetOwner())
    {
        if (Parents.Contains(Parent))
            return nullptr;
        Parents.Add(Parent);
        if (Cast<AInventoryItem>(Parent) && ++ContainerDepth > 2)
        {
            UE_LOG(LogInventoryModule, Warning, TEXT("Default inventory nesting exceeds supported depth."));
            return nullptr;
        }
    }

    FActorSpawnParameters Params;

    Params.Owner = GetOwner();
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return GetWorld()->SpawnActor<AInventoryItem>(ItemClass, GetOwner()->GetActorTransform(), Params);
}

void UInventoryManagerComponent::ClearInventory()
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;

    TArray<AInventoryItem*> Items;

    for (UInventoryItemSlot* Slot : InventorySlots)
        if (Slot && Slot->InventoryItem.IsValid()) Items.Add(Slot->InventoryItem.Get());
    for (AInventoryItem* Item : Items) DropItem(Item);
    // Clear entries that represented unspawned items.
    InventorySlots.Empty();
    NativeOnInventoryUpdated();
}

AInventoryItem* UInventoryManagerComponent::GetEquippedItem()
{
    for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
        if (IsValid(Slot) && Slot->Config.SlotKind == EInventoryEquipmentSlotKind::MainHand)
            return Slot->GetAttachedItem();
    return nullptr;
}

//================================================
//====================VICINITY====================
//================================================

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
        VicinityCheckParams.bAutoIgnoreRecursive = true;

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

//================================================
//================EQUIPMENT SLOTS=================
//================================================

void UInventoryManagerComponent::InitializeEquipmentSlots()
{
    if (bEquipmentInitialized || !GetOwner() || !GetOwner()->HasAuthority())
        return;
    bEquipmentInitialized = true;

    TSet<FName> IDs;

    for (const FInventoryEquipmentSlotConfig& Entry : EquipmentSlotConfigs)
    {
        if (Entry.SlotID.IsNone() || IDs.Contains(Entry.SlotID))
        {
            UE_LOG(LogInventoryModule, Warning, TEXT("Ignoring empty or duplicate equipment slot ID."));
            continue;
        }
        IDs.Add(Entry.SlotID);

        UClass* SlotClass = EquipmentSlotComponentClass ? EquipmentSlotComponentClass.Get() : UInventoryAttachmentComponent::StaticClass();
        UInventoryAttachmentComponent* Slot = NewObject<UInventoryAttachmentComponent>(GetOwner(), SlotClass, MakeUniqueObjectName(GetOwner(), SlotClass, Entry.SlotID), RF_Transient);

        Slot->Config = Entry;
        Slot->InventoryManager = this;
        if (Slot->Config.SlotKind == EInventoryEquipmentSlotKind::MainHand && Slot->Config.AttachmentSocket.IsNone())
            if (AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(GetOwner()))
                Slot->Config.AttachmentSocket = Character->RightHandSocketName;
        EquipmentSlots.Add(Slot);
        Slot->RegisterComponent();
    }
    // Registration never spawns items; only this manager owns default initialization.
    for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
        if (Slot->Config.DefaultItemClass)
        {
            AInventoryItem* Item = SpawnInventoryItem(Slot->Config.DefaultItemClass);

            if (Item && !CommitEquipment(Item, Slot) && !AddItem(Item))
                UE_LOG(LogInventoryModule, Warning, TEXT("Default equipment %s could not fit; left in world."), *GetNameSafe(Item));
        }
}

UInventoryAttachmentComponent* UInventoryManagerComponent::FindEquipmentSlot(AInventoryItem* Item, bool bIncludeHands) const
{
    if (!IsValid(Item))
        return nullptr;
    // Specialized slots across this manager and its directly equipped containers come before hands.
    for (UInventoryManagerComponent* Manager : GetSearchManagers())
        if (!Manager->WouldCreateContainmentCycle(Item))
            for (UInventoryAttachmentComponent* Slot : Manager->EquipmentSlots)
                if (IsValid(Slot) && Slot->Config.SlotKind == EInventoryEquipmentSlotKind::Equipment && Slot->CanAcceptItem(Item))
                    return Slot;
    if (bIncludeHands && !WouldCreateContainmentCycle(Item))
        for (EInventoryEquipmentSlotKind Kind : {EInventoryEquipmentSlotKind::MainHand, EInventoryEquipmentSlotKind::OffHand})
            for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
                if (IsValid(Slot) && Slot->Config.SlotKind == Kind && Slot->CanAcceptItem(Item))
                    return Slot;
    return nullptr;
}

UInventoryAttachmentComponent* UInventoryManagerComponent::GetEquipmentSlot(FName ID)
{
    for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
        if (IsValid(Slot) && Slot->Config.SlotID == ID)
            return Slot;
    return nullptr;
}

UInventoryAttachmentComponent* UInventoryManagerComponent::GetEquipmentSlotByClass(TSubclassOf<AInventoryItem> ItemClass)
{
    if (!ItemClass)
        return nullptr;
    for (UInventoryAttachmentComponent* Slot : EquipmentSlots)
        if (IsValid(Slot) && Slot->Config.AcceptedItemClass && ItemClass->IsChildOf(Slot->Config.AcceptedItemClass))
            return Slot;
    return nullptr;
}

//================================================
//===================EQUIPPING====================
//================================================

void UInventoryManagerComponent::Equip(AInventoryItem* Item)
{
    TryEquipItem(Item);
}

void UInventoryManagerComponent::EquipToEquipmentSlot(AInventoryItem* Item, UInventoryAttachmentComponent* EquipmentSlot)
{
    CommitEquipment(Item, EquipmentSlot);
}

/*callback function - should be triggered by the item itself*/
void UInventoryManagerComponent::OnEquipFinished(AInventoryItem* Item)
{
    if (!IsValid(Item))
        return;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || !Component->AttachmentSlot)
        return;
    if (Component->AttachmentSlot->Config.SlotKind == EInventoryEquipmentSlotKind::MainHand)
        EquippedItem = Item;
    PendingEquip = nullptr;
    OnItemEquipped.Broadcast(Item);
}

void UInventoryManagerComponent::Unequip(AInventoryItem* Item)
{
    TryPutAwayItem(Item);
}

void UInventoryManagerComponent::OnUnequipFinished(AInventoryItem* Item)
{
    if (EquippedItem == Item) EquippedItem = nullptr;
    NativeOnInventoryUpdated();
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

bool UInventoryManagerComponent::TryEquipItem(AInventoryItem* Item)
{
    UInventoryAttachmentComponent* Slot = FindEquipmentSlot(Item);

    return Slot && Slot->InventoryManager && Slot->InventoryManager->CommitEquipment(Item, Slot, Cast<AFirstPersonCharacter>(GetOwner()));
}

bool UInventoryManagerComponent::TryPutAwayItem(AInventoryItem* Item)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return false;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || !Component->AttachmentSlot)
        return false;

    const TArray<UInventoryManagerComponent*> Accessible = GetSearchManagers();
    UInventoryManagerComponent* Destination = Component->PreviousStorage.Get();

    if (!Accessible.Contains(Destination) || !IsValid(Destination) || !Destination->CanStoreLocally(Item))
        Destination = FindStorageForItem(Item);
    if (!Destination)
        return false;
    // A successful put-away releases the equipment slot only after a destination has been validated.
    UInventoryManagerComponent* Holder = Component->InventoryManager;
    IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(Item);
    AFirstPersonCharacter* User = Cast<AFirstPersonCharacter>(GetOwner());

    if (Interface) Interface->StartUnequipping(User, Component, Holder);

    const bool bStored = Destination->CommitStorage(Item);

    if (Interface) Interface->StopUnequipping(User, Component, Holder, bStored ? EInventoryItemOperationEndReason::Completed : EInventoryItemOperationEndReason::Interrupted);
    if (bStored) NativeOnInventoryUpdated();
    return bStored;
}

bool UInventoryManagerComponent::CommitEquipment(AInventoryItem* Item, UInventoryAttachmentComponent* Slot, AFirstPersonCharacter* User)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !bCanEquipItems || !IsValid(Slot) ||
        Slot->InventoryManager != this || !EquipmentSlots.Contains(Slot) || !Slot->CanAcceptItem(Item) ||
        WouldCreateContainmentCycle(Item))
        return false;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component)
        return false;

    UInventoryManagerComponent* Previous = Component->InventoryManager;

    if (Previous) Previous->ReleaseItem(Item);
    RegisterItem(Item);
    Slot->AttachedItem = Item;
    Component->AttachmentSlot = Slot;
    Component->SetItemPresence(EItemPresence::Attached);
    Component->DisableCollision();
    Slot->ApplyAttachment();
    if (Slot->Config.SlotKind == EInventoryEquipmentSlotKind::MainHand) EquippedItem = Item;
    if (Previous && Previous != this) Previous->NativeOnInventoryUpdated();
    Component->BeginEquip(User);
    NativeOnInventoryUpdated();
    return true;
}

//================================================
//===================QUICKSLOTS===================
//================================================

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
		OnPrimaryAssigned.Broadcast(this, GetOwner(), PrimaryWeapon);
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
		OnSecondaryAssigned.Broadcast(this, GetOwner(), SecondaryWeapon);
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
		OnAlternativeAssigned.Broadcast(this, GetOwner(), AlternativeWeapon);
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

//================================================
//================ITEM MANAGEMENT=================
//================================================

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
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return false;

    UInventoryManagerComponent* Destination = FindStorageForItem(Item);

    return Destination && Destination->CommitStorage(Item);
}

bool UInventoryManagerComponent::RemoveItem(AInventoryItem* Item)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !IsValid(Item))
        return false;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || Component->InventoryManager != this)
        return false;
    ReleaseItem(Item);
    Component->SetItemPresence(EItemPresence::World);
    Component->EquipState = EEquipState::NONE;
    Item->SetOwner(nullptr);
    NativeOnInventoryUpdated();
    return true;
}

bool UInventoryManagerComponent::CanPickup(AInventoryItem* Item, bool bLongHeld)
{
    if (!IsValid(Item))
        return false;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || Component->GetItemPresence() != EItemPresence::World ||
        !IInventoryItemInterface::Execute_CanBePickedUp(Item))
        return false;
    return bLongHeld ? FindEquipmentSlot(Item) != nullptr : FindStorageForItem(Item) != nullptr;
}

bool UInventoryManagerComponent::PickupItem(AInventoryItem* Item, const FInteractionContext& Context)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !CanPickup(Item, Context.bLongHeld))
        return false;
    if (!(Context.bLongHeld ? TryEquipItem(Item) : AddItem(Item)))
        return false;
    OnItemPickedUp.Broadcast(this, GetOwner(), Item);
    if (IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(Item))
        Interface->OnItemPickedUp(GetOwner(), Context);
    NativeOnInventoryUpdated();
    return true;
}

void UInventoryManagerComponent::DropItem(AInventoryItem* Item)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !IsValid(Item))
        return;

    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || Component->InventoryManager != this)
        return;
    if (IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(Item))
        Interface->StartDrop(Cast<AFirstPersonCharacter>(GetOwner()), Component, this);
    if (!RemoveItem(Item))
        return;
    if (IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(Item))
    {
        Interface->OnItemDropped(GetOwner());
        Interface->StopDrop(Cast<AFirstPersonCharacter>(GetOwner()), Component, this, EInventoryItemOperationEndReason::Completed);
    }
}

void UInventoryManagerComponent::StoreItem(AInventoryItem* Item)
{
    AddItem(Item);
}

void UInventoryManagerComponent::RegisterItem(AInventoryItem* Item)
{
    if (UInventoryItemComponent* Component = GetItemComponent(Item))
    {
        Item->SetOwner(GetOwner());
        Component->InventoryManager = this;
    }
}

UInventoryManagerComponent* UInventoryManagerComponent::FindStorageForItem(AInventoryItem* Item) const
{
    for (UInventoryManagerComponent* Manager : GetSearchManagers())
        if (IsValid(Manager) && Manager->CanStoreLocally(Item))
            return Manager;
    return nullptr;
}

bool UInventoryManagerComponent::CanStoreLocally(AInventoryItem* Item) const
{
    if (!IsValid(Item) || !Item->FindComponentByClass<UInventoryItemComponent>() || WouldCreateContainmentCycle(Item))
        return false;
    for (UInventoryItemSlot* Slot : InventorySlots)
        if (Slot && Slot->InventoryItem == Item)
            return true;
    return GetItemSlotSize(Item->GetItemSize()) <= GetRemainingSlotCapacity();
}

bool UInventoryManagerComponent::WouldCreateContainmentCycle(AInventoryItem* Item) const
{
    TSet<const UInventoryManagerComponent*> Visited;
    const UInventoryManagerComponent* Current = this;

    while (Current)
    {
        if (Visited.Contains(Current) || Current->GetOwner() == Item)
            return true;
        Visited.Add(Current);

        const AInventoryItem* Parent = Cast<AInventoryItem>(Current->GetOwner());
        const UInventoryItemComponent* ParentItem = Parent ? Parent->FindComponentByClass<UInventoryItemComponent>() : nullptr;

        Current = ParentItem ? ParentItem->InventoryManager : nullptr;
    }
    return false;
}

UInventoryItemComponent* UInventoryManagerComponent::GetItemComponent(AInventoryItem* Item)
{
	if (!Item)
		return nullptr;

	return IsValid(Item) ? Item->FindComponentByClass<UInventoryItemComponent>() : nullptr;
}

void UInventoryManagerComponent::NativeOnInventoryUpdated()
{
    UWorld* World = GetWorld();

    // World subsystems and Blueprint/UI listeners may already be gone during component destruction.
    if (IsGarbageCollecting() || HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) || !IsValid(World) || World->bIsTearingDown)
        return;

    if (UInventorySubsystem* Subsystem = World->GetSubsystem<UInventorySubsystem>())
        Subsystem->NativeOnSelectedInventoryManagerUpdated(this);

    if (OnInventoryUpdated.IsBound())
        OnInventoryUpdated.Broadcast();
}

TArray<UInventoryManagerComponent*> UInventoryManagerComponent::GetSearchManagers() const
{
    TArray<UInventoryManagerComponent*> Result;
    TArray<UInventoryAttachmentComponent*> Ordered = EquipmentSlots;

    const auto SortSlotsByPriority = [](const UInventoryAttachmentComponent& A, const UInventoryAttachmentComponent& B)
    {
        return A.Config.StoragePriority > B.Config.StoragePriority;
    };

    Result.Add(const_cast<UInventoryManagerComponent*>(this));
    Ordered.RemoveAll([](UInventoryAttachmentComponent* Slot) { return !IsValid(Slot); });
    Ordered.StableSort(SortSlotsByPriority);
    for (UInventoryAttachmentComponent* Slot : Ordered)
        if (IsValid(Slot))
            if (UInventoryManagerComponent* Child = Slot->GetAttachedInventoryManager())
                Result.AddUnique(Child);
    return Result;
}

bool UInventoryManagerComponent::CommitStorage(AInventoryItem* Item)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || !CanStoreLocally(Item))
        return false;
    if (ContainsItem(Item))
        return true;

    UInventoryItemComponent* Component = GetItemComponent(Item);
    UInventoryManagerComponent* Previous = Component->InventoryManager;

    // Allocate destination before releasing the source. No failure paths mutate the item.
    UInventoryItemSlot* Entry = NewObject<UInventoryItemSlot>(this);

    if (!Entry)
        return false;
    Entry->InventoryItem = Item;
    Entry->ItemClass = Item->GetClass();
    Entry->SetOwner(this);

    if (Previous) Previous->ReleaseItem(Item);
    InventorySlots.Add(Entry);
    RegisterItem(Item);
    Component->EquipState = EEquipState::NONE;
    Component->SetItemPresence(EItemPresence::Storage);
    Component->DisableCollision();
    if (IInventoryItemInterface* Interface = Cast<IInventoryItemInterface>(Item))
        Interface->OnItemStored(GetOwner());
    if (Previous && Previous != this) Previous->NativeOnInventoryUpdated();
    NativeOnInventoryUpdated();
    return true;
}

void UInventoryManagerComponent::ReleaseItem(AInventoryItem* Item)
{
    UInventoryItemComponent* Component = GetItemComponent(Item);

    if (!Component || Component->InventoryManager != this)
        return;
    if (Component->ItemPresence == EItemPresence::Storage)
        Component->PreviousStorage = this;
    for (int32 Index = InventorySlots.Num() - 1; Index >= 0; --Index)
        if (InventorySlots[Index] && InventorySlots[Index]->InventoryItem == Item)
        {
            InventorySlots[Index]->SetOwner(nullptr);
            InventorySlots.RemoveAt(Index);
        }
    if (Component->AttachmentSlot)
    {
        Component->AttachmentSlot->AttachedItem = nullptr;
        Component->AttachmentSlot = nullptr;
    }
    if (EquippedItem == Item) EquippedItem = nullptr;
    Component->InventoryManager = nullptr;
}

//================================================
//=====================SLOTS======================
//================================================

void UInventoryManagerComponent::MoveInventorySlotToInventory(UInventoryItemSlot* InventorySlot)
{
    if (!InventorySlot || !GetOwner() || !GetOwner()->HasAuthority())
        return;
    if (InventorySlot->InventoryItem.IsValid())
    {
        AddItem(InventorySlot->InventoryItem.Get());
        return;
    }
    if (!CanInventorySlotFitInInventory(InventorySlot))
        return;

    UInventoryManagerComponent* Previous = InventorySlot->GetOwner();

    if (Previous == this)
        return;
    if (Previous) Previous->RemoveInventorySlotFromInventory(InventorySlot);
    AddInventorySlotToInventory(InventorySlot);
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
    if (!InventorySlot || !GetOwner() || !GetOwner()->HasAuthority())
        return;
    if (InventorySlot->InventoryItem.IsValid())
    {
        AddItem(InventorySlot->InventoryItem.Get());
        return;
    }
    if (InventorySlots.Contains(InventorySlot) || !CanInventorySlotFitInInventory(InventorySlot))
        return;
    InventorySlots.Add(InventorySlot);
    InventorySlot->SetOwner(this);
    InventorySlot->OnAddedToInventoryManager(this);
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
        if (!IsValid(ItemSlot)) continue;

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

/* CreateInventoryItemSlot() - Creates an Inventory Item Slot
*
*
*
*
*/
UInventoryItemSlot* UInventoryManagerComponent::CreateInventoryItemSlot(FInventoryItemSpawnParams ItemSpawnParams, bool bSpawnInstance)
{
    if (!ItemSpawnParams.ItemClass || !GetOwner() || !GetOwner()->HasAuthority())
        return nullptr;

    UInventoryItemSlot* Entry = NewObject<UInventoryItemSlot>(this);

    Entry->ItemClass = ItemSpawnParams.ItemClass;
    if (bSpawnInstance || ItemSpawnParams.bEquipOnSpawn)
    {
        AInventoryItem* Item = SpawnInventoryItem(ItemSpawnParams.ItemClass);

        if (!Item)
            return nullptr;
        Entry->InventoryItem = Item;
        if (ItemSpawnParams.bEquipOnSpawn) TryEquipItem(Item);
    }
    return Entry;
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

//================================================
//======================GRID======================
//================================================

void UInventoryManagerComponent::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

//================================================
//=======================UI=======================
//================================================

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

//================================================
//==================REGISTRATION==================
//================================================

/*register ourselves with the Inventory Subsystem so they know we exist*/
void UInventoryManagerComponent::RegisterInventoryManager()
{
	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{
		ISS->RegisterInventoryManager(this);
	}
}

