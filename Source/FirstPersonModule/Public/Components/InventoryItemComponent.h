// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Components/ActorComponent.h"
#include "InventoryItemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInventorySpawned, AActor*, Item, UInventoryItemComponent*, ItemComponent, UInventoryManagerComponent*, InventoryManager, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEquipped, AActor*, Item, UInventoryItemComponent*, ItemComponent, UInventoryManagerComponent*, InventoryManager, AActor*, OwningActor);


UCLASS(ClassGroup = (Inventory), AutoExpandCategories = ("Equipping/Unequipping|Equipping"), meta = (BlueprintSpawnableComponent), HideCategories = (Sockets, ComponentTick, ComponentReplication, Activation, Cooking, Collision, AssetUserData))
class FIRSTPERSONMODULE_API UInventoryItemComponent : public UActorComponent
{
	friend class UInventoryManagerComponent;

	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details")
		FText ItemName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details")
		FText ItemDescription;


public:
	UPROPERTY(EditAnywhere, Category = "Inventory Item")
		EItemCondition Condition = EItemCondition::Prestine;

protected:
	/*if enabled - allows item to be used with a Point of Sale system*/
	UPROPERTY(EditAnywhere, Category = "Inventory Item|Point of Sale")
		bool bSellableItem = false;
	UPROPERTY(EditAnywhere, Category = "Inventory Item|Point of Sale", meta = (EditCondition = "bSellableItem", EditConditionHides))
		float MSRP = 9.99;


	/*reference to inventory manager when assigned*/
private:
	UPROPERTY()
		class UInventoryManagerComponent* InventoryManager;
public:
	/*automatically equips this item when picked up or spawned into inventory*/
	UPROPERTY(EditAnywhere, Category = "Equipping/Unequipping")
		bool bAutoEquip = false;
	/*if called - will immedietely unhide the mesh. Otherwise it's up to the actor itself to unhide when needed*/
	UPROPERTY(EditAnywhere, Category = "Equipping/Unequipping")
		bool bAutoUnhideOnEquip = true;
	/*if the Inventory Manager already has a currently equipped item - will check to see which priority is higher to swap*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipping/Unequipping")
		uint8 EquipPriority = 0;
private:
	/**/
	UPROPERTY()
		EEquipState EquipState = EEquipState::NONE;

	FTimerHandle EquipHandler;
	FTimerHandle UnequipHandler;
	
public:
	/*the current registered Grid Key*/
	UPROPERTY()
		FIntPoint RegisteredGridKey;

private:
	/*State of the Item*/
	UPROPERTY()
		EItemState ItemState = EItemState::AsPickup;
	UPROPERTY()
		EItemState PreviousItemState = EItemState::NONE;
private:
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_CollisionChanged")
		bool bCollisionEnabled = true;


	/*events/delegates*/
public:
	UPROPERTY(BlueprintAssignable)
		FOnInventorySpawned OnItemSpawned;
	UPROPERTY(BlueprintAssignable)
		FOnEquipped OnEquipBegin;
	UPROPERTY(BlueprintAssignable)
		FOnEquipped OnEquipEnd;
	UPROPERTY(BlueprintAssignable)
		FOnEquipped OnUnequipBegin;
	UPROPERTY(BlueprintAssignable)
		FOnEquipped OnUnequipEnd;
	UPROPERTY(BlueprintAssignable)
		FOnEquipped OnPickedUp;

	//===========================================================================================================================================================
	//=========================================================================FUNCTIONS=========================================================================
	//===========================================================================================================================================================

public:
	// Sets default values for this component's properties
	UInventoryItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//=================================
	//===============POS===============
	//=================================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Point of Sale")
		bool IsSellable();


	//====================================
	//===============STATES===============
	//====================================

public:
	UFUNCTION(BlueprintPure, Category = "State")
		EEquipState GetEquipState() { return EquipState; };
	UFUNCTION(BlueprintPure, Category = "State")
		EItemState GetItemState() { return ItemState; };

private:
	UFUNCTION()
		void SetItemState(EItemState NewState);


	//=================================
	//============EQUIPPING============
	//=================================
public:
	/*this typically is called by the InventoryManager & routes down to the owning Actor*/
	UFUNCTION()
		virtual void BeginEquip();
	/*EndEquip() then gets called back from the owning actor after it's done its equip logic to finalize the states of the ItemComp*/
	UFUNCTION()
		virtual void EndEquip();
	UFUNCTION()
		virtual void BeginUnequip();
	UFUNCTION()
		virtual void EndUnequip();

	////===============================
	//============PICKUPS==============
	////===============================
public:
	UFUNCTION(BlueprintPure, Category = "Interaction")
		virtual bool CanPickup();

	/*this function isn't called directly - but called via picking up an item through the InventoryManager*/
private:
	UFUNCTION()
		virtual void PickedUp(AActor* Instigator, UInventoryManagerComponent* InstigatorInvManager = nullptr);
	/*event called when dropped on the ground*/
	UFUNCTION()
		virtual void OnDropped();


public:
	UFUNCTION(BlueprintPure, Category = "Inventory Manager")
		class UInventoryManagerComponent* GetInventoryManager() { return InventoryManager; };
	UFUNCTION(BlueprintPure, Category = "Inventory Manager")
		class AActor* GetOwningActor();

public:
	UFUNCTION()
		void UpdateGridKey(FIntPoint NewGridKey);
	UFUNCTION()
		virtual void RegisterGridLocation();
	UFUNCTION()
		virtual void UnregisterGridLocation();


private:
	UFUNCTION()
		virtual void EnableCollision();
	UFUNCTION()
		virtual void DisableCollision();
	UFUNCTION()
		virtual void OnRep_CollisionChanged();
	UFUNCTION()
		virtual void EnableVisibility();
	UFUNCTION()
		virtual void DisableVisibility();
};
