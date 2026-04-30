// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Inventory/Inventory.h"
#include "InventorySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedInventoryManagerUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryViewTypeChanged, EInventoryView, NewInventoryView);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedEquipmentSlotChanged);


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:




	/*inventory management viewing*/
private:

	/*the view-type the player is currently using*/
	UPROPERTY()
		EInventoryView InventoryView = EInventoryView::Vicinity;
	UPROPERTY()
		TWeakObjectPtr<class UInventoryManagerComponent> HoveredInventoryManager;
	UPROPERTY()
		TWeakObjectPtr<class UInventoryManagerComponent> SelectedInventoryManager;

	UPROPERTY()
		TWeakObjectPtr<class UInventoryAttachmentComponent> HoveredEquipmentSlot;
	UPROPERTY()
		TWeakObjectPtr<class UInventoryAttachmentComponent> SelectedEquipmentSlot;

	//UPROPERTY()
	//	TWeakObjectPtr<class UInventoryItemSlot> HoveredInventorySlot;
	//UPROPERTY()
	//	TWeakObjectPtr<class UInventoryItemSlot> SelectedInventorySlot;


	

private:
	UPROPERTY()
		class UInventoryItemAsset* HoveredItemAsset;


private:
	UPROPERTY()
		TSet<TWeakObjectPtr<class AInventoryItem>> RegisteredInventoryItems;
	/*master list of all inventory managers*/
	UPROPERTY()
		TSet<TWeakObjectPtr<class UInventoryManagerComponent>> RegisteredInventoryManagers;

	/*grid system*/
private:
	/*spatial-grid map for determining what items on the ground or inventory managers on actors are in nearby spots*/
	UPROPERTY()
		TMap<FIntPoint, FInventoryGridEntry> InventoryGrid;

	/*sized used to group inventory/inventory managers for more optimized checks*/
	static constexpr int GridChunkSize = 10000;

private:
	/*debugging*/
	bool bDebugInventoryGrid = false;


	//==============
	//====EVENTS====
	//==============

public:
	/*automatically called if an inventory manager gets an update and happens to be selected*/
	UPROPERTY(BlueprintAssignable)
		FOnSelectedInventoryManagerUpdated OnSelectedInventoryManagerUpdated;
	UPROPERTY(BlueprintAssignable)
		FOnInventoryViewTypeChanged OnInventoryViewChanged;

	UPROPERTY(BlueprintAssignable)
		FOnSelectedEquipmentSlotChanged OnSelectedEquipmentSlotChanged;

	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================


	//==============================
	//=====INVENTORY MANAGEMENT=====
	//==============================
public: 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|UI")
		class UInventoryManagerComponent* GetHoveredInventoryManager();
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void SetHoveredInventoryManager(class UInventoryManagerComponent* InventoryManager);
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void ClearHoveredInventoryManager(class UInventoryManagerComponent* InventoryManager = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|UI")
		class UInventoryManagerComponent* GetSelectedInventoryManager();
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void SetSelectedInventoryManager(class UInventoryManagerComponent* InventoryManager);
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void ClearSelectedInventoryManager(class UInventoryManagerComponent* InventoryManager = nullptr);

	/*called automatically when an Inventory Manager's inventory is updated - allows for hooking*/
	UFUNCTION()
		virtual void NativeOnSelectedInventoryManagerUpdated(class UInventoryManagerComponent* InventoryManager);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Vicinity")
		virtual TArray<class UInventoryManagerComponent*> GetInventoryManagersNearby(const FVector& Location, const float Threshold, FInventoryManagerVicinityCheckParameters VicinityCheckParameters = FInventoryManagerVicinityCheckParameters()) const;


public:
	UFUNCTION()
		virtual void RegisterInventoryManager(class UInventoryManagerComponent* InventoryManager);
	UFUNCTION()
		virtual void UnregisterInventoryManager(class UInventoryManagerComponent* InventoryManager);
	
	//=================================
	//=========INVENTORY ITEMS=========
	//=================================
public:
	UFUNCTION()
		virtual void RegisterInventoryItem(class AInventoryItem* InventoryItem);
	UFUNCTION()
		virtual void UnregisterInventoryItem(class AInventoryItem* InventoryItem);

	//UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
	//	virtual void RemoveInventoryItemFromVicinity(class AInventoryItem* InventoryItem);

	//=================================
	//=========EQUIPMENT SLOTS=========
	//=================================
	public:
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Equipment|Slots")
			virtual class UInventoryAttachmentComponent* GetHoveredEquipmentSlot();
		UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment|Slots")
			virtual void SetHoveredEquipmentSlot(class UInventoryAttachmentComponent* Slot);
		
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Equipment|Slots")
			virtual class UInventoryAttachmentComponent* GetSelectedEquipmentSlot();
		UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment|Slots")
			virtual void SetSelectedEquipmentSlot(class UInventoryAttachmentComponent* Slot);

	//=====================================
	//===========ITEM ATTACHMENT===========
	//=====================================
public:
	//UFUNCTION(BlueprintCallable, Category = "Inventory|Attachments")
	//	virtual void 


	/*UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void TransferInventorySlot(UInventory)*/


	//=================================
	//==========ITEM VICINITY==========
	//=================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Vicinity")
		TArray<class AInventoryItem*> GetInventoryItemsInVicinity(const FVector& Location, const float Threshold);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Vicinity")
		TArray<class UInventoryItemSlot*> GetInventorySlotsInVicinity(const FVector& Location, const float Threshold);

	//===============================
	//==========ITEM ASSETS==========
	//===============================

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|UI")
		class UInventoryItemAsset* GetHoveredItemAsset();
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void SetHoveredItemAsset(class UInventoryItemAsset* ItemAsset);
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		virtual void ClearHoveredItemAsset(class UInventoryItemAsset* ItemAsset = nullptr);

public:
	/*returns the inventory item component of this class - or NULL if none*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
		class UInventoryItemComponent* GetInventoryItemComponent(AActor* Actor);

	//========================
	//=====INVENTORY VIEW=====
	//========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventroy|UI")
		EInventoryView GetInventoryView();
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
		void SetInventoryView(EInventoryView NewInventoryView);



	//========================
	//==========GRID==========
	//========================

protected:
	/*returns a converted grid key for looking up inventory items based on grid space*/
	UFUNCTION(BlueprintCallable, Category = "Loot Subsystem")
		FIntPoint GetGridKey(const FVector& Location) const;
	
	virtual void UpdateInventoryGridEntry(class UInventoryManagerComponent* InventoryManager);
	virtual void UpdateInventoryGridEntry(class UInventoryItemComponent* ItemComponent);
	virtual void UpdateInventoryGridEntry(class AInventoryItem* InventoryItem);
	virtual void RemoveInventoryGridEntry(FIntPoint GridKey, class UInventoryManagerComponent* InventoryManager);
	virtual void RemoveInventoryGridEntry(FIntPoint GridKey, class AInventoryItem* InventoryItem);
	virtual bool IsGridEntryWithinThreshold(const FVector& Location, const FIntPoint& GridKey, float ThresholdSquared) const;

	/*returns whether or not this grid already exists*/
	virtual bool GridEntryExists(FIntPoint GridKey);
	virtual FInventoryGridEntry& GetGridEntry(FIntPoint GridKey);

	//===========================
	//=========UTILITIES=========
	//===========================
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		const double GetDistanceFromInventoryItem(const FVector& Location, const class AInventoryItem* Item) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		const double GetDistanceFromInventoryManager(const FVector& Location, const UInventoryManagerComponent* Component) const;

	static void SortInventoryItemsByDistance(const FVector& Location, TArray<class AInventoryItem*>& InventoryItemList);
	static void SortInventoryManagersByDistance(const FVector& Location, TArray<class UInventoryManagerComponent*>& InventoryManagerList);

	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		class UInventoryItemSlot* CreateInventorySlotFromItem(class AInventoryItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		TArray<class UInventoryItemSlot*> CreateInventorySlotsFromItems(TArray<class AInventoryItem*> ItemList);
	

	/*returns a list of all Inventory Items (instances) from a list of Inventory Slots*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		TArray<class AInventoryItem*> GetInventoryItemsFromSlots(const TArray<UInventoryItemSlot*> InventorySlots) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Subsystem")
		TArray<class UInventoryItemSlot*> GetInventorySlotsOfSize(EItemSize Size, const TArray<UInventoryItemSlot*> InventorySlots) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		bool AreInventoryItemListsIdentical(const TArray<AInventoryItem*>& A, const TArray<AInventoryItem*>& B) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		bool AreInventorySlotListsIdentical(const TArray<UInventoryItemSlot*>& A, const TArray<UInventoryItemSlot*>& B) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		bool AreInventoryManagerListsIdentical(const TArray<class UInventoryManagerComponent*> A, const TArray<UInventoryManagerComponent*>& B) const;



	//=========================
	//========DEBUGGING========
	//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		virtual void ToggleDebugGrid();
	UFUNCTION(BlueprintCallable, Category = "Inventory Subsystem")
		virtual void ToggleDebugTable(const FVector& Location);
private:
	UFUNCTION()
		void DrawDebugGrid();
	UFUNCTION()
		void DrawDebugDataTable(const FVector& Location);




	
};
