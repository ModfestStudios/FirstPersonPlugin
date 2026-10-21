// Copyright (c) 2022 Pocket Sized Animations

#include "Interfaces/InventoryItemInterface.h"

void IInventoryItemInterface::OnItemStored(AActor* StorageActor)
{
	NativeOnItemStored(StorageActor);
	Execute_BP_OnItemStored(Cast<UObject>(this), StorageActor);
}

void IInventoryItemInterface::OnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context)
{
	NativeOnItemPickedUp(ReceivingActor, Context);
	Execute_BP_OnItemPickedUp(Cast<UObject>(this), ReceivingActor, Context);
}

void IInventoryItemInterface::OnItemDropped(AActor* RejectingActor)
{
	NativeOnItemDropped(RejectingActor);
	Execute_BP_OnItemDropped(Cast<UObject>(this), RejectingActor);
}

void IInventoryItemInterface::StartPickup(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context)
{
	NativeOnPickupStarted(User, ItemComponent, InventoryManager, Context);
	Execute_BP_OnPickupStarted(Cast<UObject>(this), User, ItemComponent, InventoryManager, Context);
}

void IInventoryItemInterface::StopPickup(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context, EInventoryItemOperationEndReason EndReason)
{
    FInteractionContext TerminalContext = Context;
    TerminalContext.bInterrupted = EndReason == EInventoryItemOperationEndReason::Interrupted;
	if (EndReason == EInventoryItemOperationEndReason::Completed)
	{
		NativeOnPickupCompleted(User, ItemComponent, InventoryManager, TerminalContext);
		Execute_BP_OnPickupCompleted(Cast<UObject>(this), User, ItemComponent, InventoryManager, TerminalContext);
	}
	else
	{

		NativeOnPickupCanceled(User, ItemComponent, InventoryManager, TerminalContext);
		Execute_BP_OnPickupCanceled(Cast<UObject>(this), User, ItemComponent, InventoryManager, TerminalContext);
	}
}

void IInventoryItemInterface::StartDrop(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
	NativeOnDroppedStarted(User, ItemComponent, InventoryManager);
	Execute_BP_OnDroppedStarted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
}

void IInventoryItemInterface::StopDrop(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason)
{
	if (EndReason == EInventoryItemOperationEndReason::Completed)
	{
		NativeOnDroppedCompleted(User, ItemComponent, InventoryManager);
		Execute_BP_OnDroppedCompleted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
	}
	else
	{
		const bool bInterrupted = EndReason == EInventoryItemOperationEndReason::Interrupted;
		NativeOnDroppedCanceled(User, ItemComponent, InventoryManager, bInterrupted);
		Execute_BP_OnDroppedCanceled(Cast<UObject>(this), User, ItemComponent, InventoryManager, bInterrupted);
	}
}

void IInventoryItemInterface::StartEquipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
	NativeOnEquippingStarted(User, ItemComponent, InventoryManager);
	Execute_BP_OnEquippingStarted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
}

void IInventoryItemInterface::StopEquipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason)
{
	if (EndReason == EInventoryItemOperationEndReason::Completed)
	{
		NativeOnEquippingCompleted(User, ItemComponent, InventoryManager);
		Execute_BP_OnEquippingCompleted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
	}
	else
	{
		const bool bInterrupted = EndReason == EInventoryItemOperationEndReason::Interrupted;
		NativeOnEquippingCanceled(User, ItemComponent, InventoryManager, bInterrupted);
		Execute_BP_OnEquippingCanceled(Cast<UObject>(this), User, ItemComponent, InventoryManager, bInterrupted);
	}
}

void IInventoryItemInterface::StartUnequipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
	NativeOnUnequippingStarted(User, ItemComponent, InventoryManager);
	Execute_BP_OnUnequippingStarted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
}

void IInventoryItemInterface::StopUnequipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason)
{
	if (EndReason == EInventoryItemOperationEndReason::Completed)
	{
		NativeOnUnequippingCompleted(User, ItemComponent, InventoryManager);
		Execute_BP_OnUnequippingCompleted(Cast<UObject>(this), User, ItemComponent, InventoryManager);
	}
	else
	{
		const bool bInterrupted = EndReason == EInventoryItemOperationEndReason::Interrupted;
		NativeOnUnequippingCanceled(User, ItemComponent, InventoryManager, bInterrupted);
		Execute_BP_OnUnequippingCanceled(Cast<UObject>(this), User, ItemComponent, InventoryManager, bInterrupted);
	}
}

void IInventoryItemInterface::NativeOnItemStored(AActor* StorageActor)
{
}

void IInventoryItemInterface::NativeOnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context)
{
}

void IInventoryItemInterface::NativeOnItemDropped(AActor* RejectingActor)
{
}

void IInventoryItemInterface::NativeOnDroppedStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnDroppedCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnDroppedCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted)
{
}

void IInventoryItemInterface::NativeOnPickupStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context)
{
}

void IInventoryItemInterface::NativeOnPickupCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context)
{
}

void IInventoryItemInterface::NativeOnPickupCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context)
{
}

void IInventoryItemInterface::NativeOnEquippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnEquippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnEquippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted)
{
}

void IInventoryItemInterface::NativeOnUnequippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnUnequippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager)
{
}

void IInventoryItemInterface::NativeOnUnequippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted)
{
}

