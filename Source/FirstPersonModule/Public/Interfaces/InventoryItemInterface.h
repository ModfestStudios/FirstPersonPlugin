// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inventory/Inventory.h"
#include "Interactives/Interactives.h"
#include "InventoryItemInterface.generated.h"

class AFirstPersonCharacter;
class AActor;
class UInventoryItemComponent;
class UInventoryManagerComponent;



UINTERFACE(MinimalAPI)
class UInventoryItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Owner callbacks for actors with an InventoryItemComponent.
 * The component owns operation state and timing; these callbacks notify the actor.
 * Dispatchers invoke both native and Blueprint hooks, as in InteractiveActorInterface.
 */
class FIRSTPERSONMODULE_API IInventoryItemInterface
{
	GENERATED_BODY()

	friend class UInventoryItemComponent;

public:
	// Inventory membership notifications, separate from timed operation callbacks.
	void OnItemStored(AActor* StorageActor);
	void OnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context);
	void OnItemDropped(AActor* RejectingActor);

	// Item-specific pickup eligibility; inventory capacity is validated by the manager.
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory Item|Pickup")
	bool CanBePickedUp() const;
	virtual bool CanBePickedUp_Implementation() const
	{
		return true;
	}

	// Called by the inventory system after updating operation state.
	// Override the NativeOn hooks below rather than these dispatchers.
	// Every accepted start should receive exactly one terminal notification.
	void StartPickup(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);
	void StopPickup(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context, EInventoryItemOperationEndReason EndReason);

	void StartDrop(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	void StopDrop(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason);

	void StartEquipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	void StopEquipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason);

	void StartUnequipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	void StopUnequipping(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, EInventoryItemOperationEndReason EndReason);

protected:
	// Native actor hooks. Default implementations do nothing.
	virtual void NativeOnItemStored(AActor* StorageActor);
	virtual void NativeOnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context);
	virtual void NativeOnItemDropped(AActor* RejectingActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Storage", meta = (DisplayName = "On Item Stored", BlueprintProtected))
	void BP_OnItemStored(AActor* StorageActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Pickup", meta = (DisplayName = "On Item Picked Up", BlueprintProtected))
	void BP_OnItemPickedUp(AActor* ReceivingActor, const FInteractionContext& Context);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Drop", meta = (DisplayName = "On Item Dropped", BlueprintProtected))
	void BP_OnItemDropped(AActor* RejectingActor);

	virtual void NativeOnDroppedStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnDroppedCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnDroppedCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);

	virtual void NativeOnPickupStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);
	virtual void NativeOnPickupCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);
	virtual void NativeOnPickupCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);

	virtual void NativeOnEquippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnEquippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnEquippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);

	virtual void NativeOnUnequippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnUnequippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);
	virtual void NativeOnUnequippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);

	// Blueprint hooks are dispatched automatically after their native counterparts.
	// bInterrupted is true for an interruption and false for an explicit cancellation.
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Pickup", meta = (DisplayName = "On Pickup Started", BlueprintProtected))
	void BP_OnPickupStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Pickup", meta = (DisplayName = "On Pickup Completed", BlueprintProtected))
	void BP_OnPickupCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Pickup", meta = (DisplayName = "On Pickup Canceled", BlueprintProtected))
	void BP_OnPickupCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, const FInteractionContext& Context);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Equipping", meta = (DisplayName = "On Equipping Started", BlueprintProtected))
	void BP_OnEquippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Equipping", meta = (DisplayName = "On Equipping Completed", BlueprintProtected))
	void BP_OnEquippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Equipping", meta = (DisplayName = "On Equipping Canceled", BlueprintProtected))
	void BP_OnEquippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Unequipping", meta = (DisplayName = "On Unequipping Started", BlueprintProtected))
	void BP_OnUnequippingStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Unequipping", meta = (DisplayName = "On Unequipping Completed", BlueprintProtected))
	void BP_OnUnequippingCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Unequipping", meta = (DisplayName = "On Unequipping Canceled", BlueprintProtected))
	void BP_OnUnequippingCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Drop", meta = (DisplayName = "On Dropped Started", BlueprintProtected))
	void BP_OnDroppedStarted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Drop", meta = (DisplayName = "On Dropped Completed", BlueprintProtected))
	void BP_OnDroppedCompleted(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Item|Drop", meta = (DisplayName = "On Dropped Canceled", BlueprintProtected))
	void BP_OnDroppedCanceled(AFirstPersonCharacter* User, UInventoryItemComponent* ItemComponent, UInventoryManagerComponent* InventoryManager, bool bInterrupted);
};
