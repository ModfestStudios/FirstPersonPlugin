// Copyright (c) 2022 Pocket Sized Animations

/*inventory*/
#include "Components/InventoryManagerComponent.h"
#include "Inventory/InventoryItem.h"
#include "Components/InventoryItemComponent.h"
#include "Subsystems/LootSubsystem.h"
#include "Inventory/InventoryLoadout.h"

/*logging*/
#include "../Logging.h"

/*replication*/
#include "Net/UnrealNetwork.h"

/*ui*/
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/InventoryWidgetBase.h"

/*setup replication*/
void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagerComponent, CurrentlyEquipped);
	DOREPLIFETIME(UInventoryManagerComponent, PrimaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, SecondaryWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, AlternativeWeapon);
	DOREPLIFETIME(UInventoryManagerComponent, Inventory);
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
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	/*spawn the default inventory*/
	if (Inventory.Num() == 0)//we check if Inventory == 0 because BeginPlay() is called multiple times, so this saves overhead
		SpawnDefaultInventory();
}

//void UInventoryManagerComponent::SpawnInventoryFromTemplate(AInventoryTemplate* InventoryTemplate, bool bClearExisting)
//{
//	/*server-check*/
//	if (GetNetMode() == NM_Client)
//		return;
//
//	if (!InventoryTemplate)
//		return;
//
//	if (bClearExisting)
//		ClearInventory();
//
//	///*spawn the primary/secondary/alternative weapons*/
//	//if (auto PrimWeap = InventoryTemplate->PrimaryWeapon)
//	//{
//	//	PrimaryWeapon = SpawnInventoryItem(PrimWeap);
//	//	AddItem(PrimaryWeapon);
//
//	//	if (ShouldAutoEquip(PrimaryWeapon))
//	//		Equip(PrimaryWeapon);
//	//}
//	//if (auto SecWeap = InventoryTemplate->SecondaryWeapon)
//	//{
//	//	SecondaryWeapon = SpawnInventoryItem(SecWeap);
//	//	AddItem(SecondaryWeapon);
//	//}
//	//if (auto AltWeap = InventoryTemplate->AlternativeWeapon)
//	//{
//	//	AlternativeWeapon = SpawnInventoryItem(AltWeap);
//	//	AddItem(AlternativeWeapon);
//	//}
//
//
//
//	//SpawnInventory(InventoryTemplate->Inventory, bClearExisting);
//}

void UInventoryManagerComponent::SpawnDefaultInventory()
{
	/*server check*/
	if (GetNetMode() == NM_Client)
		return;

	if (IsValid(DefaultPrimary))
	{
		AssignPrimaryWeapon(SpawnInventoryItem(DefaultPrimary));

		if (bAutoEquipPrimaryOnSpawn)
			Equip(GetPrimaryWeapon());
	}

	if (IsValid(DefaultSecondary))
		AssignSecondaryWeapon(SpawnInventoryItem(DefaultSecondary));
	if (IsValid(DefaultAlternative))
		AssignAlternativeWeapon(SpawnInventoryItem(DefaultAlternative));
	if (DefaultItems.IsEmpty() == false)
		SpawnInventory(DefaultItems, true);
}

void UInventoryManagerComponent::SpawnInventoryFromLoadout(AInventoryLoadout* Loadout, bool bClearExisting)
{
	/*servery check*/
	if (GetNetMode() == NM_Client)
		return;

	/*safety check*/
	if(!IsValid(Loadout))
		return;

	TSubclassOf<AActor> Primary = Loadout->PrimaryWeapon.ItemClass;
	//TArray<TSubclassOf<AActor>> InventoryItems = Loadout->Items;

	if (Primary)
	{
		AActor* NewPrimary = SpawnInventoryItem(Primary);
		AssignPrimaryWeapon(NewPrimary);

		if (bAutoEquipPrimaryOnSpawn)
			Equip(GetPrimaryWeapon());
	}

	//if (InventoryItems.Num() > 0)
		//SpawnInventory(InventoryItems, false);

}

void UInventoryManagerComponent::SpawnInventory(TArray<TSubclassOf<AActor>> Items, bool bClearExisting)
{
	/*safety/server-check*/
	if (GetOwner() == nullptr || GetOwner()->IsPendingKillPending() || GetNetMode() == NM_Client)
		return;

	/*destroy existing inventory*/
	if (bClearExisting)
		ClearInventory();

	/*auto-equip*/
	AActor* AutoEquip = nullptr;

	for (TSubclassOf<AActor> ItemClass : Items)
	{
		/*skip blank entries*/
		if (ItemClass == nullptr)
			continue;

		/*spawn item*/
		AActor* Item = SpawnInventoryItem(ItemClass);

		if (Item)
		{
			if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
			{
				AddItem(ItemComp->GetOwner());


				if (ShouldAutoEquip(Item))
					AutoEquip = Item;
			}
		}
	}

	if (AutoEquip != nullptr)
		Equip(AutoEquip);
}

AActor* UInventoryManagerComponent::SpawnInventoryItem(TSubclassOf<AActor> ItemClass)
{
	/*safety/server-check*/
	if (GetOwner() == nullptr || GetNetMode() == NM_Client)
		return nullptr;

	/*initialize*/
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetOwner()->GetActorLocation());
	SpawnTransform.SetRotation(GetOwner()->GetActorRotation().Quaternion());
	SpawnTransform.SetScale3D(FVector(1));

	return GetWorld()->SpawnActor<AActor>(ItemClass, SpawnTransform, SpawnParams);
}

void UInventoryManagerComponent::ClearInventory()
{

}

AActor* UInventoryManagerComponent::GetCurrentlyEquippedItem()
{
	return CurrentlyEquipped;
}

TArray<UInventoryItemComponent*> UInventoryManagerComponent::GetItemsInVicinity()
{
	if (!GetOwner() || GetOwner()->IsPendingKillPending())
		return TArray<UInventoryItemComponent*>();

	TArray<UInventoryItemComponent*> Items;
	FVector OwnerLocation = GetOwner()->GetActorLocation();

	if (ULootSubsystem* LootSubsystem = GetWorld()->GetSubsystem<ULootSubsystem>())
	{		
		Items = LootSubsystem->GetItemsInVicinty(OwnerLocation, MaxVicinityItemDistance);
	}

	return Items;
}


//=====================================
//==============EQUIPPING==============
//=====================================

void UInventoryManagerComponent::Equip(AActor* Item)
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

/*callback function - should be triggered by the item itself*/
void UInventoryManagerComponent::OnEquipFinished(AActor* Item)
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

void UInventoryManagerComponent::Unequip(AActor* Item)
{
	/*safetry check*/
	if (Item == nullptr)
		return;

	if (UInventoryItemComponent* ItemComp = GetItemComponent(Item))
	{
		ItemComp->BeginUnequip();
	}
}

void UInventoryManagerComponent::OnUnequipFinished(AActor* Item)
{
	CurrentlyEquipped = nullptr;

	/*if we're waiting to equip anything - go ahead*/
	if (PendingEquip)
	{
		Equip(PendingEquip);
	}
}

bool UInventoryManagerComponent::ShouldAutoEquip(AActor* Item, bool bItemPickedUp)
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


AActor* UInventoryManagerComponent::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

void UInventoryManagerComponent::AssignPrimaryWeapon(AActor* Weapon)
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

AActor* UInventoryManagerComponent::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void UInventoryManagerComponent::AssignSecondaryWeapon(AActor* Weapon)
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

AActor* UInventoryManagerComponent::GetAlternativeWeapon()
{
	return AlternativeWeapon;
}

void UInventoryManagerComponent::AssignAlternativeWeapon(AActor* Weapon)
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



void UInventoryManagerComponent::AssignToQuickslot(AActor* ActorToAssign, EQuickslot Quickslot)
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

AActor* UInventoryManagerComponent::GetQuickslotItem(EQuickslot Quickslot)
{
	return Quickslots[(uint8)Quickslot];
}



//=======================================
//============ITEM MANAGEMENT============
//=======================================
bool UInventoryManagerComponent::ContainsItem(AActor* Item)
{
	/*safety check*/
	if (!Item)
		return false;

	if (Inventory.Contains(Item))
		return true;
	else
		return false;
}

bool UInventoryManagerComponent::AddItem(AActor* Item)
{
	if (!Item)
		return false;


	/*add to inventory list*/
	int32 i = Inventory.Add(Item);
	if (i >= 0)
	{
		/*set the Item's ownering to our owner && provide InventoryManager reference*/
		RegisterItem(Item);
		/*disables collision & hides model*/
		StoreItem(Item);

		if (OnItemAdded.IsBound())
			OnItemAdded.Broadcast(this, GetOwner(), Item);

		return true;
	}
	else
		return false;
}

bool UInventoryManagerComponent::RemoveItem(AActor* Item)
{
	if (!Item)
		return false;

	Item->SetOwner(nullptr);

	return false;
}

void UInventoryManagerComponent::PickupItem(AActor* Item)
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
		OnItemPickedUp.Broadcast(this, GetOwner(), Item);

	/*notify item it's been picked up*/
	if (auto ItemComp = GetItemComponent(Item))
		ItemComp->PickedUp(GetOwner(), this);
}

bool UInventoryManagerComponent::CanPickup(AActor* Item)
{
	if (auto ItemComp = GetItemComponent(Item))
		return ItemComp->CanPickup();

	/*fallback*/
	return false;
}

void UInventoryManagerComponent::DropItem(AActor* Item)
{
	/*safety check*/
	if (!Item)
		return;

	if (auto ItemComp = GetItemComponent(Item))
		ItemComp->OnDropped();

	/*make sure we remove it from us*/
	RemoveItem(Item);
}


void UInventoryManagerComponent::StoreItem(AActor* Item)
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
		//ItemComp->DisableVisibility();
	}
}

void UInventoryManagerComponent::RegisterItem(AActor* Item)
{
	if (!Item)
		return;

	if (UInventoryItemComponent* ItemComp = Cast<UInventoryItemComponent>(Item->GetComponentByClass(UInventoryItemComponent::StaticClass())))
	{
		Item->SetOwner(GetOwner());
		ItemComp->InventoryManager = this;
	}
}

void UInventoryManagerComponent::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

UInventoryItemComponent* UInventoryManagerComponent::GetItemComponent(AActor* Item)
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
	}
}

//==============================
//==============UI==============
//==============================
void UInventoryManagerComponent::OpenInventoryUI()
{		
	if (InventoryWidget)
	{
		InventoryWidget->AddToPlayerScreen();
		InventoryWidget->SetFocus();
	}
}

void UInventoryManagerComponent::CloseInventoryUI()
{
	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
	}
}

bool UInventoryManagerComponent::IsInventoryUIOpen()
{
	if (!InventoryWidget)
		return false;	

	return InventoryWidget->IsInViewport();
}

