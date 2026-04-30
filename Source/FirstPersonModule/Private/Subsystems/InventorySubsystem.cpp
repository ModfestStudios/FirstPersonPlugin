// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "Subsystems/InventorySubsystem.h"
#include "Inventory/InventoryItemAsset.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryAttachmentComponent.h"
#include "Inventory/InventoryItemSlot.h"
#include "Inventory/InventoryEquipmentSlot.h"

UInventoryManagerComponent* UInventorySubsystem::GetHoveredInventoryManager()
{
	if (HoveredInventoryManager.IsValid())
		return HoveredInventoryManager.Get();
	else
		return nullptr;
}

void UInventorySubsystem::SetHoveredInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	HoveredInventoryManager = InventoryManager;
}

void UInventorySubsystem::ClearHoveredInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	if (InventoryManager == nullptr || InventoryManager == HoveredInventoryManager)
		HoveredInventoryManager = nullptr;

	NativeOnSelectedInventoryManagerUpdated(InventoryManager);
}

UInventoryManagerComponent* UInventorySubsystem::GetSelectedInventoryManager()
{
	if (SelectedInventoryManager.IsValid())
		return SelectedInventoryManager.Get();
	else
		return nullptr;
}

void UInventorySubsystem::SetSelectedInventoryManager(UInventoryManagerComponent* InventoryManager)
{	
	if (!InventoryManager || InventoryManager == SelectedInventoryManager)
		return;

	SelectedInventoryManager = InventoryManager;	
	NativeOnSelectedInventoryManagerUpdated(InventoryManager);
}

void UInventorySubsystem::ClearSelectedInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	if (InventoryManager == nullptr || InventoryManager == SelectedInventoryManager)
		SelectedInventoryManager = nullptr;
}

void UInventorySubsystem::NativeOnSelectedInventoryManagerUpdated(UInventoryManagerComponent* InventoryManager)
{
	/*event notification*/
	if (InventoryManager == SelectedInventoryManager && OnSelectedInventoryManagerUpdated.IsBound())
		OnSelectedInventoryManagerUpdated.Broadcast();
}

TArray<class UInventoryManagerComponent*> UInventorySubsystem::GetInventoryManagersNearby(const FVector& Location,const float Threshold, FInventoryManagerVicinityCheckParameters VicinityCheckParameters) const
{	
	TArray<UInventoryManagerComponent*> NearbyInvManagers;

	const float ThresholdSquared = Threshold * Threshold;
	const FIntPoint GridKey = GetGridKey(Location); 
	const int32 CellRadius = FMath::CeilToInt(Threshold / GridChunkSize); //Round up to the nearest integer

	for (int32 XOffset =- CellRadius; XOffset <= CellRadius; ++XOffset)
	{
		for (int32 YOffset =- CellRadius; YOffset <= CellRadius; ++YOffset)
		{
			const FIntPoint NeighborKey(GridKey.X + XOffset, GridKey.Y + YOffset);

			if (!IsGridEntryWithinThreshold(Location, NeighborKey, ThresholdSquared))
			{
				continue;
			}

			const FInventoryGridEntry* GridEntry = InventoryGrid.Find(NeighborKey);
			if (!GridEntry)
			{
				continue;
			}

			for (const TWeakObjectPtr<UInventoryManagerComponent>& WeakManager : GridEntry->Managers)
			{
				UInventoryManagerComponent* InvManager = WeakManager.Get();

				if (!IsValid(InvManager))
				{
					continue;
				}

				AActor* Owner = InvManager->GetOwner();

				/*skip invalid actors or ones on the ignore list*/
				if (!IsValid(Owner) || VicinityCheckParameters.IgnoredActors.Contains(Owner))
				{
					continue;
				}

				const FVector ManagerLocation = Owner->GetActorLocation();
				const double Distance = GetDistanceFromInventoryManager(Location, InvManager);
				

				if (Distance <= ThresholdSquared)
				{
					NearbyInvManagers.Add(InvManager);
				}
			}
		}
	}

	/*do a quick sort*/
	SortInventoryManagersByDistance(Location, NearbyInvManagers);


	return NearbyInvManagers;
}



void UInventorySubsystem::RegisterInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	/*safety checks*/
	if (!IsValid(InventoryManager))
		return;
	
	/*add to registry list (auto-checks for dupes)*/
	RegisteredInventoryManagers.Add(InventoryManager);

	UpdateInventoryGridEntry(InventoryManager);
}

void UInventorySubsystem::UnregisterInventoryManager(UInventoryManagerComponent* InventoryManager)
{
	if (InventoryManager && !RegisteredInventoryManagers.Contains(InventoryManager))
		RegisteredInventoryManagers.Remove(InventoryManager);
}


//AInventoryItem* UInventorySubsystem::SpawnInventoryItem(FInventoryItemSpawnParams SpawnParameters, FVector SpawnLocation, FRotator SpawnRotation)
//{
//	/*safety/server-check*/
//	if (!GetWorld() || GetWorld()->GetNetMode() == NM_Client || SpawnParameters.ItemClass == nullptr)
//		return;
//
//	/*initialize*/
//	TSubclassOf<AInventoryItem> ItemClass = SpawnParameters.ItemClass;
//	FActorSpawnParameters SpawnParams;
//	//SpawnParams.
//
//
//
//	return nullptr;
//
//	
//}

void UInventorySubsystem::RegisterInventoryItem(AInventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem))
		return;

	RegisteredInventoryItems.Add(InventoryItem);

	UpdateInventoryGridEntry(InventoryItem);
}

void UInventorySubsystem::UnregisterInventoryItem(AInventoryItem* InventoryItem)
{
	
}

UInventoryAttachmentComponent* UInventorySubsystem::GetHoveredEquipmentSlot()
{
	if(HoveredEquipmentSlot.IsValid())
		return HoveredEquipmentSlot.Get();
	else
		return nullptr;
}

void UInventorySubsystem::SetHoveredEquipmentSlot(UInventoryAttachmentComponent* Slot)
{
	HoveredEquipmentSlot = Slot;
}

UInventoryAttachmentComponent* UInventorySubsystem::GetSelectedEquipmentSlot()
{
	if (SelectedEquipmentSlot.IsValid())
		return SelectedEquipmentSlot.Get();
	else
		return nullptr;
}

void UInventorySubsystem::SetSelectedEquipmentSlot(UInventoryAttachmentComponent* Slot)
{
	SelectedEquipmentSlot = Slot;

	if (OnSelectedEquipmentSlotChanged.IsBound())
		OnSelectedEquipmentSlotChanged.Broadcast();
}



TArray<class AInventoryItem*> UInventorySubsystem::GetInventoryItemsInVicinity(const FVector& Location, const float Threshold)
{
	TArray<AInventoryItem*> NearbyInventoryItems;

	const float ThresholdSquared = Threshold * Threshold;
	const FIntPoint GridKey = GetGridKey(Location);
	const int32 CellRadius = FMath::CeilToInt(Threshold / GridChunkSize); //Round up to the nearest integer

	for (int32 XOffset = -CellRadius; XOffset <= CellRadius; ++XOffset)
	{
		for (int32 YOffset = -CellRadius; YOffset <= CellRadius; ++YOffset)
		{
			const FIntPoint NeighborKey(GridKey.X + XOffset, GridKey.Y + YOffset);

			if (!IsGridEntryWithinThreshold(Location, NeighborKey, ThresholdSquared))
			{
				continue;
			}

			const FInventoryGridEntry* GridEntry = InventoryGrid.Find(NeighborKey);
			if (!GridEntry)
			{
				continue;
			}

			for (const TWeakObjectPtr<AInventoryItem>& WeakItem : GridEntry->InventoryItems)
			{
				AInventoryItem* Item = WeakItem.Get();

				if (!IsValid(Item))
				{
					continue;
				}
								
				const double Distance = GetDistanceFromInventoryItem(Location, Item);

				if (Item->CanBePickedUp())
				{

				}
				/*perform checks*/
				if (Item->CanBePickedUp())
				{
					if (Distance <= ThresholdSquared)
					{
						NearbyInventoryItems.Add(Item);
					}
				}
				
				//continue;
			}
		}
	}

	/*do a quick sort*/
	SortInventoryItemsByDistance(Location, NearbyInventoryItems);
	//SortInventoryManagersByDistance(Location, NearbyInvManagers);


	return NearbyInventoryItems;
}

TArray<class UInventoryItemSlot*> UInventorySubsystem::GetInventorySlotsInVicinity(const FVector& Location, const float Threshold)
{
	TArray<AInventoryItem*> InventoryItems = GetInventoryItemsInVicinity(Location, Threshold);
	TArray<UInventoryItemSlot*> ItemSlots = CreateInventorySlotsFromItems(InventoryItems);

	return ItemSlots;
}


UInventoryItemAsset* UInventorySubsystem::GetHoveredItemAsset()
{
	return HoveredItemAsset;
}

void UInventorySubsystem::SetHoveredItemAsset(UInventoryItemAsset* ItemAsset)
{
	HoveredItemAsset = ItemAsset;
}

void UInventorySubsystem::ClearHoveredItemAsset(UInventoryItemAsset* ItemAsset)
{	
	/*if no ItemAsset is passed - just clear*/
	if (ItemAsset == nullptr)
		HoveredItemAsset = nullptr;

	/*if we passed an ItemAsset - confirm the hovered one is the same and clear if we find a match*/
	if (ItemAsset == HoveredItemAsset)
		HoveredItemAsset = nullptr;

}

UInventoryItemComponent* UInventorySubsystem::GetInventoryItemComponent(AActor* Actor)
{
	if (!Actor)
		return nullptr;

	return Actor->GetComponentByClass<UInventoryItemComponent>();
}

EInventoryView UInventorySubsystem::GetInventoryView()
{
	return InventoryView;
}

void UInventorySubsystem::SetInventoryView(EInventoryView NewInventoryView)
{
	if (NewInventoryView == InventoryView)
		return;
		
	InventoryView = NewInventoryView;

	if (OnInventoryViewChanged.IsBound())
		OnInventoryViewChanged.Broadcast(NewInventoryView);
}

FIntPoint UInventorySubsystem::GetGridKey(const FVector& Location) const
{
	return FIntPoint(
		FMath::FloorToInt(Location.X / GridChunkSize),
		FMath::FloorToInt(Location.Y / GridChunkSize)
	);
}


void UInventorySubsystem::UpdateInventoryGridEntry(UInventoryManagerComponent* InventoryManager)
{
	/*safety checks*/
	if (!IsValid(InventoryManager))
		return;

	/*init*/
	FVector ManagerLocation = InventoryManager->GetOwner()->GetActorLocation(); //location of owning actor
	FIntPoint GridKey = GetGridKey(ManagerLocation); //Grid key based on location
	FIntPoint OldGridKey = InventoryManager->RegisteredGridKey; //Old key

	/*check if exists already*/
	if (GridKey == OldGridKey && GridEntryExists(GridKey)) //cancels out if already added
		return;

	/*find or add a new entry*/
	FInventoryGridEntry& GridEntry = GetGridEntry(GridKey);

	/*update any containers in the list*/
	GridEntry.Managers.AddUnique(InventoryManager);

	/*remove any old references*/
	if (GridKey != OldGridKey && InventoryGrid.Contains(OldGridKey))
		RemoveInventoryGridEntry(GridKey, InventoryManager);

	/*update gridkey*/
	InventoryManager->UpdateGridKey(GridKey);
}

void UInventorySubsystem::UpdateInventoryGridEntry(UInventoryItemComponent* ItemComponent)
{

}

void UInventorySubsystem::UpdateInventoryGridEntry(AInventoryItem* InventoryItem)
{
	if (!InventoryItem)
		return;

	FVector ItemLocation = InventoryItem->GetActorLocation();
	FIntPoint GridKey = GetGridKey(ItemLocation);
	FIntPoint OldGridKey = InventoryItem->RegisteredGridKey;

	if (GridKey == OldGridKey && GridEntryExists(GridKey))
		return;

	FInventoryGridEntry& GridEntry = GetGridEntry(GridKey);

	GridEntry.InventoryItems.AddUnique(InventoryItem);

	if (GridKey != OldGridKey && InventoryGrid.Contains(OldGridKey))
		RemoveInventoryGridEntry(GridKey, InventoryItem);

	InventoryItem->UpdateGridKey(GridKey);
}

void UInventorySubsystem::RemoveInventoryGridEntry(FIntPoint GridKey, UInventoryManagerComponent* InventoryManager)
{
	FInventoryGridEntry* GridEntry = InventoryGrid.Find(GridKey);
	GridEntry->Managers.Remove(InventoryManager);
}

void UInventorySubsystem::RemoveInventoryGridEntry(FIntPoint GridKey, AInventoryItem* InventoryItem)
{
	FInventoryGridEntry* GridEntry = InventoryGrid.Find(GridKey);
	GridEntry->InventoryItems.Remove(InventoryItem);
}

bool UInventorySubsystem::IsGridEntryWithinThreshold(const FVector& Location, const FIntPoint& GridKey, float ThresholdSquared) const
{
	// Calculate the bounds of the grid based on GridKey
	FVector MinCorner(GridKey.X * GridChunkSize, GridKey.Y * GridChunkSize, 0);
	FVector MaxCorner((GridKey.X + 1) * GridChunkSize, (GridKey.Y + 1) * GridChunkSize, 0);

	// Clamp the location to the grid's bounds
	FVector ClampedLocation = Location;
	ClampedLocation.X = FMath::Clamp(Location.X, MinCorner.X, MaxCorner.X);
	ClampedLocation.Y = FMath::Clamp(Location.Y, MinCorner.Y, MaxCorner.Y);

	// Compute squared distance from the clamped point to the player's location
	float DistSquared = FVector::DistSquared(ClampedLocation, Location);

	// Return whether the grid is within the threshold
	return DistSquared <= ThresholdSquared;
}

bool UInventorySubsystem::GridEntryExists(FIntPoint GridKey)
{
	return InventoryGrid.Contains(GridKey);
}

FInventoryGridEntry& UInventorySubsystem::GetGridEntry(FIntPoint GridKey)
{
	return InventoryGrid.FindOrAdd(GridKey);
}




//===========================
//=========UTILITIES=========
//===========================


const double UInventorySubsystem::GetDistanceFromInventoryItem(const FVector& Location, const AInventoryItem* Item) const
{
	const FVector ItemLocation = Item->GetActorLocation();
	const double Distance = FVector::DistSquared(ItemLocation, Location);

	return Distance;
}

const double UInventorySubsystem::GetDistanceFromInventoryManager(const FVector& Location, const UInventoryManagerComponent* Component) const
{
	const FVector ComponentLocation = Component->GetOwner() ? Component->GetOwner()->GetActorLocation() : FVector(9999999);
	const double Distance = FVector::DistSquared(ComponentLocation, Location);
	
	return Distance;
}


void UInventorySubsystem::SortInventoryItemsByDistance(const FVector& Location, TArray<class AInventoryItem*>& InventoryItemList)
{
	InventoryItemList.Sort([&Location](const AInventoryItem& A, const AInventoryItem& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), Location) < FVector::DistSquared(B.GetActorLocation(), Location);
		});
}

/*sorts the list based on who's closest to the given location*/
void UInventorySubsystem::SortInventoryManagersByDistance(const FVector& Location, TArray<class UInventoryManagerComponent*>& InventoryManagerList)
{
	InventoryManagerList.Sort([&Location](const UInventoryManagerComponent& A, const UInventoryManagerComponent& B)
		{
			const AActor* OwnerA = A.GetOwner();
			const AActor* OwnerB = B.GetOwner();

			if (!OwnerA || !OwnerB)
			{
				return OwnerA != nullptr;
			}

			return FVector::DistSquared(OwnerA->GetActorLocation(), Location)
				< FVector::DistSquared(OwnerB->GetActorLocation(), Location);
		});
}

UInventoryItemSlot* UInventorySubsystem::CreateInventorySlotFromItem(AInventoryItem* Item)
{
	if (!Item)
		return nullptr;

	UInventoryItemSlot* ItemSlot = NewObject<UInventoryItemSlot>();

	ItemSlot->InventoryItem = Item;
	ItemSlot->ItemClass = Item->GetClass();

	return ItemSlot;
}

TArray<class UInventoryItemSlot*> UInventorySubsystem::CreateInventorySlotsFromItems(TArray<class AInventoryItem*> ItemList)
{
	TArray<UInventoryItemSlot*> ItemSlots;

	for (int32 i = 0; i < ItemList.Num(); i++)
	{
		ItemSlots.Add(CreateInventorySlotFromItem(ItemList[i]));
	}

	return ItemSlots;
}

TArray<class AInventoryItem*> UInventorySubsystem::GetInventoryItemsFromSlots(const TArray<UInventoryItemSlot*> InventorySlots) const
{
	TArray<AInventoryItem*> Items;

	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{			
		if (!IsValid(InventorySlots[i]) || !InventorySlots[i]->InventoryItem.IsValid())
			continue;

		AInventoryItem* InventoryItem = InventorySlots[i]->InventoryItem.Get();
		
		Items.Add(InventoryItem);
	}

	return Items;
}

TArray<class UInventoryItemSlot*> UInventorySubsystem::GetInventorySlotsOfSize(EItemSize Size, const TArray<UInventoryItemSlot*> InventorySlots) const
{
	TArray<UInventoryItemSlot*> FilteredItems;

	for (UInventoryItemSlot* Item : InventorySlots)
	{
		if (!Item)
			continue;

		if (UInventoryItemComponent* ItemComp = Item->ItemClass->GetDefaultObject<AInventoryItem>()->GetItemComponent())
		{
			/*check size*/
			if (ItemComp->Size == Size)
				FilteredItems.Add(Item);
		}
	}

	return FilteredItems;
}

bool UInventorySubsystem::AreInventoryItemListsIdentical(const TArray<AInventoryItem*>& A, const TArray<AInventoryItem*>& B) const
{
	if(A.Num() != B.Num())
		return false;

	/*copy lists so we can sort them without modifying original*/
	TArray<AInventoryItem*> ListA = A;
	TArray<AInventoryItem*> ListB = B;

	/*sort to compare*/
	ListA.Sort();
	ListB.Sort();

	return ListA == ListB;
}

bool UInventorySubsystem::AreInventorySlotListsIdentical(const TArray<UInventoryItemSlot*>& A, const TArray<UInventoryItemSlot*>& B) const
{
	if (A.Num() != B.Num())
		return false;

	TArray<AInventoryItem*> AItems = GetInventoryItemsFromSlots(A);
	TArray<AInventoryItem*> BItems = GetInventoryItemsFromSlots(B);

	AItems.Sort();
	BItems.Sort();

	if (AItems == BItems)
		return true;
	else
		return false;
}

bool UInventorySubsystem::AreInventoryManagerListsIdentical(const TArray<class UInventoryManagerComponent*> A, const TArray<UInventoryManagerComponent*>& B) const
{
	if (A.Num() != B.Num())
		return false;

	TArray<UInventoryManagerComponent*> ListA = A;
	TArray<UInventoryManagerComponent*> ListB = B;

	ListA.Sort();
	ListB.Sort();

	return ListA == ListB;
}


//=========================
//========DEBUGGING========
//=========================

void UInventorySubsystem::ToggleDebugGrid()
{
	bDebugInventoryGrid = true;

	DrawDebugGrid();
}

void UInventorySubsystem::ToggleDebugTable(const FVector& Location)
{
	DrawDebugDataTable(Location);
}

void UInventorySubsystem::DrawDebugGrid()
{
	/*loop through all existing grids and draw them*/
	for (const auto& GridEntry : InventoryGrid)
	{
		FVector Center = FVector(
			(GridEntry.Key.X + 0.5f) * GridChunkSize,
			(GridEntry.Key.Y + 0.5f) * GridChunkSize,
			0);

		FVector Extents(GridChunkSize * 0.5f, GridChunkSize * 0.5f, 100);

		DrawDebugBox(GetWorld(), Center, Extents, FColor::Emerald, false, 3.0f, 0, 5);
	}
}

void UInventorySubsystem::DrawDebugDataTable(const FVector& Location)
{
	if (!GEngine)
		return;

	const FIntPoint GridKey = GetGridKey(Location);
	const FInventoryGridEntry& GridEntry = GetGridEntry(GridKey);

	FString DebugText = TEXT("Grid.X | Grid.Y | Manager | Distance | # Items \n");

	for (const TWeakObjectPtr<UInventoryManagerComponent>& ManagerPtr : GridEntry.Managers)
	{
		if (!ManagerPtr.IsValid())
			continue;

		UInventoryManagerComponent* Manager = ManagerPtr.Get();

		DebugText += FString::Printf(
			TEXT("%6d | %6d | %s\n"),
			GridKey.X,
			GridKey.Y,
			*Manager->GetName()			
		);
	}

	GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Emerald, DebugText);
}




