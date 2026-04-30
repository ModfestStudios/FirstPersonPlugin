// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/InventoryWidget.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/InventoryItemComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/PlayerInventoryManagerComponent.h"

/*subsystems*/
#include "Subsystems/InventorySubsystem.h"

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

void UInventoryWidget::InitializeInventoryWidget(UInventoryManagerComponent* InventoryManagerComponent)
{
	//InventoryManager = InventoryManagerComponent;
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UInventorySubsystem* ISS = GetWorld()->GetSubsystem<UInventorySubsystem>())
	{		
		ISS->OnSelectedInventoryManagerUpdated.AddUniqueDynamic(this, &UInventoryWidget::OnSelectedInventoryManagerUpdated);
		ISS->OnInventoryViewChanged.AddUniqueDynamic(this, &UInventoryWidget::OnInventoryViewChanged);		
	}

	if (AFirstPersonCharacter* Char = GetOwningCharacter())
	{
		if (UInventoryManagerComponent* InvManager = Char->GetInventoryManager())
		{
			InvManager->OnInventoryManagerOpened.AddUniqueDynamic(this, &UInventoryWidget::OnInventoryManagerOpened);
			InvManager->OnInventoryManagerClosed.AddUniqueDynamic(this, &UInventoryWidget::OnInventoryManagerClosed);
			InvManager->OnInventoryItemsInVicinityUpdated.AddUniqueDynamic(this, &UInventoryWidget::NativeOnInventoryItemsInVicinityUpdated);
			InvManager->OnInventoryManagersInVicinityUpdated.AddUniqueDynamic(this, &UInventoryWidget::NativeOnInventoryManagersInVicinityUpdated);
		}
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
		
}

void UInventoryWidget::NativeDestruct()
{	
	Super::NativeDestruct();
}

void UInventoryWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}


UInventoryManagerComponent* UInventoryWidget::GetInventoryManager()
{
	if(AFirstPersonCharacter* Character = GetOwningCharacter())
	{
		return Character->GetInventoryManager();		
	}	

	return nullptr;
}

AFirstPersonCharacter* UInventoryWidget::GetOwningCharacter()
{
	return GetOwningPlayerPawn<AFirstPersonCharacter>();
}

void UInventoryWidget::OnInventoryManagerOpened()
{
	BP_OnInventoryManagerOpened();
}

void UInventoryWidget::OnInventoryManagerClosed()
{
	
	BP_OnInventoryManagerClosed();
}

void UInventoryWidget::NativeOnInventoryManagersInVicinityUpdated()
{
	BP_OnInventoryManagersInVicinityUpdated();
}

void UInventoryWidget::NativeOnInventoryItemsInVicinityUpdated()
{
	BP_OnInventoryItemsInVicinityUpdated();
}

void UInventoryWidget::OnSelectedInventoryManagerUpdated()
{
	/*notify blueprint hook*/
	BP_OnSelectedInventoryManagerUpdated();
}

void UInventoryWidget::CloseInventory()
{
	if (AFirstPersonCharacter* PlayerCharacter = GetOwningCharacter())
	{
		PlayerCharacter->GetInventoryManager()->CloseInventoryWidget();
	}
}

void UInventoryWidget::OnInventoryViewChanged(EInventoryView NewInventoryView)
{

	BP_OnInventoryViewChanged(NewInventoryView);
}

UInventorySubsystem* UInventoryWidget::GetInventorySubsystem()
{
	if (!CachedInventorySubsystem)
		CachedInventorySubsystem = GetWorld()->GetSubsystem<UInventorySubsystem>();
	
	return CachedInventorySubsystem;
	
}
