// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/LootSubsystem.h"

#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"

/*framework*/
#include "GameFramework/Actor.h"

void ULootSubsystem::RegisterDroppedItem(UInventoryItemComponent* ItemComponent)
{
    if (!ItemComponent || !ItemComponent->GetOwner())
        return;

    FVector ItemLocation = ItemComponent->GetOwner()->GetActorLocation();
    FIntPoint GridKey = GetGridKey(ItemLocation);
    FIntPoint OldGridKey = ItemComponent->RegisteredGridKey;

    /*key is already exists so no need to continue*/
    if (GridKey == OldGridKey && InventoryGrid.Contains(GridKey))
        return;
    
    
    FInventoryGridData& GridData = InventoryGrid.FindOrAdd(GridKey);
    GridData.Items.AddUnique(ItemComponent);

    /*cleanup old entries*/    
    if (InventoryGrid.Contains(OldGridKey))
        UnregisterDroppedItem(ItemComponent, OldGridKey);

    /*update the item's grid key*/
    ItemComponent->UpdateGridKey(GridKey);
}

void ULootSubsystem::UnregisterDroppedItem(UInventoryItemComponent* ItemComponent, FIntPoint GridKey)
{
    if (!ItemComponent)
        return;

    if (InventoryGrid.Contains(GridKey))
    {
        FInventoryGridData* GridData = InventoryGrid.Find(GridKey);
        GridData->Items.Remove(ItemComponent);
    }
}

void ULootSubsystem::RegisterInventoryManager(UInventoryManagerComponent* InventoryManager)
{
    if (!InventoryManager || !InventoryManager->GetOwner() || !InventoryManager->GetOwner()->IsPendingKillPending())
        return;

    /*init*/
    FVector ManagerLocation = InventoryManager->GetOwner()->GetActorLocation();
    FIntPoint GridKey = GetGridKey(ManagerLocation);
    FIntPoint OldGridKey = InventoryManager->RegisteredGridKey;

    /*check if exists already*/
    if (GridKey == OldGridKey && InventoryGrid.Contains(GridKey))
        return;

    /*add entry*/
    FInventoryGridData& GridData = InventoryGrid.FindOrAdd(GridKey);
    GridData.Managers.AddUnique(InventoryManager);

    if (InventoryGrid.Contains(OldGridKey))
        UnregisterInventoryManager(InventoryManager, OldGridKey);

    /*update gridkey*/
    InventoryManager->UpdateGridKey(GridKey);
}

void ULootSubsystem::UnregisterInventoryManager(UInventoryManagerComponent* InventoryManager, FIntPoint GridKey)
{
    if (!InventoryManager)
        return;

    if (InventoryGrid.Contains(GridKey))
    {
        FInventoryGridData* GridData = InventoryGrid.Find(GridKey);
        GridData->Managers.Remove(InventoryManager);
    }
}

const TArray<UInventoryItemComponent*> ULootSubsystem::GetItemsInVicinty(const FVector& Location, const float Threshold)
{
    TArray<UInventoryItemComponent*> Items;
    float ThresholdSquared = Threshold * Threshold;

    // Get the grid key for the given location
    FIntPoint GridKey = GetGridKey(Location);

    // Iterate over neighboring grids, including the current grid
    for (int32 XOffset = -1; XOffset <= 1; ++XOffset)
    {
        for (int32 YOffset = -1; YOffset <= 1; ++YOffset)
        {
            // Calculate the neighboring grid key
            FIntPoint NeighborKey = FIntPoint(GridKey.X + XOffset, GridKey.Y + YOffset);

            // Check if this grid is worth searching
            if (!IsGridWithinThreshold(Location, NeighborKey, ThresholdSquared))
                continue;

            // Check items in the grid
            FInventoryGridData* GridData = InventoryGrid.Find(NeighborKey);
            if (!GridData)
                continue;

            for (auto ItemComp : GridData->Items)
            {
                if (!ItemComp || !ItemComp->GetOwner() || ItemComp->GetOwner()->IsPendingKillPending())
                    continue;

                if (ItemComp->GetOwner())
                {
                    FVector ItemLocation = ItemComp->GetOwner()->GetActorLocation();

                    if (FVector::DistSquared(ItemLocation, Location) <= ThresholdSquared)
                    {
                        Items.Add(ItemComp);
                    }
                }
            }
        }
    }

    return Items;
}

bool ULootSubsystem::IsGridWithinThreshold(const FVector& Location, const FIntPoint& GridKey, float ThresholdSquared) const
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

FIntPoint ULootSubsystem::GetGridKey(const FVector& Location) const
{
    return FIntPoint(
        FMath::FloorToInt(Location.X / GridChunkSize),
        FMath::FloorToInt(Location.Y / GridChunkSize)
    );
}
