// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Lists/InventoryItemList.h"
#include "Inventory/InventoryItemSlot.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Subsystems/InventorySubsystem.h"

void UInventoryItemList::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryItemList::SetInventoryManagerComponent(UInventoryManagerComponent* NewInventoryManager, bool bSetListVicinityItems)
{
    if (InventoryManager == NewInventoryManager && bListItemsInVicinity == bSetListVicinityItems)
        return;

    bListItemsInVicinity = bSetListVicinityItems;
    
    

    /*if we're already bound to an inventory manager, cleanup references*/
    if (InventoryManager)
    {
        InventoryManager->OnInventoryUpdated.RemoveDynamic(this, &UInventoryItemList::OnInventoryItemListUpdated);
    }

    if (NewInventoryManager)
    {        
        /*set the new reference*/
        InventoryManager = NewInventoryManager;

        /*bind event to the appropriate source*/
        if (bListItemsInVicinity)
            InventoryManager->OnInventoryItemsInVicinityUpdated.AddUniqueDynamic(this, &UInventoryItemList::OnInventoryItemListUpdated);
        else
            InventoryManager->OnInventoryUpdated.AddUniqueDynamic(this, &UInventoryItemList::OnInventoryItemListUpdated);          

        /*call the first update*/
        OnInventoryItemListUpdated();
    }    
}

void UInventoryItemList::OnInventoryItemListUpdated()
{

    /*cleanup old list*/
    /*for (UInventoryItemSlot* ItemSlot : InventorySlots)
    {
        ItemSlot->BeginDestroy();
    }*/

    InventorySlots.Empty();

    /*populate the slots from surrounding items*/
    if (bListItemsInVicinity)
    {
        if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
        {
            InventorySlots = ISS->CreateInventorySlotsFromItems(InventoryManager->GetInventoryItemsInVicinity());
        }
    }
    /*then we can just grab the slots directly*/
    else
    {
        InventorySlots = InventoryManager->GetInventorySlots();
    }
    

    BP_OnInventoryItemListUpdated();
}

const TArray<class UInventoryItemSlot*> UInventoryItemList::GetInventorySlots() const
{
    return InventorySlots;
}

const TArray<class UInventoryItemSlot*> UInventoryItemList::GetInventorySlotsOfSize(EItemSize Size) const
{
    TArray<UInventoryItemSlot*> ItemSlots;

    for (auto Item : InventorySlots)
    {
        if (Item->GetItemSize() == Size)
            ItemSlots.Add(Item);
    }

    return ItemSlots;
}

