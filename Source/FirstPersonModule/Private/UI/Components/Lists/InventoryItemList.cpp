// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Lists/InventoryItemList.h"
#include "Components/ListView.h"

void UInventoryItemList::NativeConstruct()
{
    Super::NativeConstruct();

    RefreshItemList();
}

void UInventoryItemList::RefreshItemList()
{
    if (ListView)
        ListView->ClearListItems();

    // Iterate through the ItemList
    for (const TSubclassOf<AActor>& ItemClass : ItemList)
    {
        if (ItemClass)
        {
            // Get the default object of the actor class
            AActor* DefaultActor = ItemClass->GetDefaultObject<AActor>();
            if (DefaultActor)
            {
                if (ListView)
                    ListView->AddItem(DefaultActor);
            }
        }
    }
}
