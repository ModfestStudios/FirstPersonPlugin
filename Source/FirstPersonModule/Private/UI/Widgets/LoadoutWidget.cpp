// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/LoadoutWidget.h"

#include "Inventory/InventoryLoadout.h"

void ULoadoutWidget::CreateNewLoadout()
{
	if (GetOwningPlayer() && GetOwningPlayer()->GetNetMode() < NM_Client)
	{
		Loadout = GetWorld()->SpawnActor<AInventoryLoadout>();
	}
}
