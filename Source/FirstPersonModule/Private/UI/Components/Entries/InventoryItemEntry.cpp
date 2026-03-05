// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/Entries/InventoryItemEntry.h"
#include "Components/InventoryItemComponent.h"

void UInventoryItemEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (AActor* Actor = Cast<AActor>(ListItemObject))
	{
		ItemActor = Actor;
		ItemComponent = Cast<UInventoryItemComponent>(Actor->GetComponentByClass(UInventoryItemComponent::StaticClass()));
	}

	/*call Blueprint version*/
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}
