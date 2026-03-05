// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/InventoryWidgetBase.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"

/*umg*/
#include "UI/Components/Entries/InventorySlotEntry.h"
#include "Components/GridPanel.h"

/*utilities*/
#include "TimerManager.h"
#include "../Logging.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//VicinityGridWidget = nullptr;
	SetIsFocusable(true);
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bCheckForItemsInVicinity)
		InitializeItemVicinityCheck();
}

void UInventoryWidget::NativeDestruct()
{
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetWorld())
		return;

	/*clear timer so it's not running when we're not being rendered*/
	GetOwningPlayer()->GetWorld()->GetTimerManager().ClearTimer(ItemVicinityHandler);
	
	Super::NativeDestruct();
}

void UInventoryWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (bCheckForItemsInVicinity)
		InitializeItemVicinityCheck();
}

void UInventoryWidget::InitializeItemVicinityCheck()
{
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetWorld() || !bCheckForItemsInVicinity)
	{
		UE_LOG(LogFirstPersonModule, Warning, TEXT("!!!%s::InitializeItemVicinityCheck() - FAILED TO INITIALIZE WIDGET"),*GetName());
		return;
	}

	/*manually call it for an instance update update*/
	UpdateItemsInVicinityList();

	/*setup a timer*/
	GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(ItemVicinityHandler, this, &UInventoryWidget::UpdateItemsInVicinityList, ItemCheckRate, true);
}

void UInventoryWidget::UpdateItemsInVicinityList()
{
	if (!VicinityGridWidget)
		return;

	UE_LOG(LogFirstPersonModule, Log, TEXT("%s::UpdateItemsInVicinityList()"), *GetName());

	/*clear previous list*/
	VicinityGridWidget->ClearChildren();

	/*get all the items around us*/
	TArray<UInventoryItemComponent*> ItemsInVicinity = GetItemsInVicinity();
	
	for (UInventoryItemComponent* Item : ItemsInVicinity)
	{
		if (!Item)
			continue;

		/*create widget for this*/
		UInventorySlotEntry* InventorySlot = CreateWidget<UInventorySlotEntry>(GetOwningPlayer(), InventorySlotClass);
		if (InventorySlot)
		{
			InventorySlot->InitializeInventorySlot(Item);

			/*@todo: check the type of grid and add the index appropriately*/
			VicinityGridWidget->AddChild(InventorySlot);
		}
	}
}

TArray<class UInventoryItemComponent*> UInventoryWidget::GetItemsInVicinity()
{
	/*safety check*/
	if (!GetOwningCharacter() || !!GetOwningCharacter()->IsPendingKillPending())
		return TArray<UInventoryItemComponent*>();

	if (AFirstPersonCharacter* Character = GetOwningCharacter())
	{
		return Character->GetInventoryManager()->GetItemsInVicinity();
	}

	return TArray<class UInventoryItemComponent*>();
}

AFirstPersonCharacter* UInventoryWidget::GetOwningCharacter()
{
	return GetOwningPlayerPawn<AFirstPersonCharacter>();
}
