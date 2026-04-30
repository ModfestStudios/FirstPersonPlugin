// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Components/ActorComponent.h"
#include "InventoryManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, UInventoryManagerComponent*, InventoryManager, class AInventoryItem*, OwningActor, class AInventoryItem*, InventoryItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, class AInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryManagerOpened);




UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent), HideCategories = (Sockets, ComponentTick, ComponentReplication, Activation, Cooking, Collision, AssetUserData))
class FIRSTPERSONMODULE_API UInventoryManagerComponent : public UActorComponent
{
	friend class UInventoryItemComponent;

	GENERATED_BODY()
public:


protected:
	/*what's currently active for this manager*/
	UPROPERTY(Replicated)
		class AInventoryItem* CurrentlyEquipped;
	UPROPERTY(Replicated)
		class AInventoryItem* PrimaryWeapon;
	UPROPERTY(Replicated)
		class AInventoryItem* AlternativeWeapon;
	UPROPERTY(Replicated)
		class AInventoryItem* SecondaryWeapon;
private:
	/*used to track an item that wants to be equipped but waiting on an action to complete*/
	UPROPERTY()
		class AInventoryItem* PendingEquip;

public:
	/*Primary Weapon - must implement PrimaryWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnablePrimaryWeapon", EditConditionHides, MustImplement = "PrimaryWeaponInterface", DisplayName = "Primary Weapon"))
		TSubclassOf<class AInventoryItem> DefaultPrimary;
	/*Primary Weapon - must implement SecondaryWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnableSecondaryWeapon", EditConditionHides, MustImplement = "SecondaryWeaponInterface", DisplayName = "Secondary Weapon"))
		TSubclassOf<class AInventoryItem> DefaultSecondary;
	/*Primary Weapon - must implement AlternativeWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnableAlternativeWeapon", EditConditionHides, MustImplement = "AlternativeWeaponInterface", DisplayName = "Alternative Weapon"))
		TSubclassOf<class AInventoryItem> DefaultAlternative;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (MustImplement = "InventoryItemInterface", DisplayName = "Inventory", TitleProperty = "DisplayName"))
		TArray<FInventoryItemSpawnParams> DefaultItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		uint8 SlotCapacity = 20;


	/*the name of this inventory manager name (typically representing the container's name itself) used for UI*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		FText InventoryManagerName = NSLOCTEXT("Inventory", "DefaultInventoryManagerName", "Unnamed Inventory Manager");

	/*equipment that can be attached to this inventory item*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", Instanced, meta = (ShowOnlyInnerProperties))
		TArray<class UInventoryEquipmentSlot*> EquipmentSlots;


	//=========================
	//=====INVENTORY ITEMS=====
	//=========================


protected:
	//UPROPERTY(Replicated)
	//	TArray<class AInventoryItem*> Inventory;

	UPROPERTY(Replicated)
		TArray<TObjectPtr<class UInventoryItemSlot>> InventorySlots;

	UPROPERTY()
		TArray<class AInventoryItem*> Quickslots;

	//===================
	//=====EQUIPMENT=====
	//===================


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
		bool bCanEquipItems;


	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Primary", meta = (EditCondition = "bCanEquipItems"))
		bool bEnablePrimaryWeapon;

	/*automatically equips Primary as the CurrentWeapon during initial spawning*/
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Primary", meta = (EditConditino = "bEnablePrimaryWeapon"))
		bool bAutoEquipPrimaryOnSpawn;


	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Secondary", meta = (EditCondition = "bCanEquipItems"))
		bool bEnableSecondaryWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Alternative", meta = (EditCondition = "bCanEquipItems"))
		bool bEnableAlternativeWeapon;



	//==================
	//=====VICINITY=====
	//==================
public:
	/*enables tick-based item vicinity checking*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Vicinity")
		bool bCheckForItemsInVincinityOnTick = false;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Vicinity")
		bool bCheckForManagersInVicinityOnTick = false;

protected:	
	UPROPERTY()
		TArray<class AInventoryItem*> ItemsInVicinity;
	UPROPERTY()
		TArray<class UInventoryManagerComponent*> ManagersInVicinity;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Tracing")
		float MaxVicinityItemDistance = 250.0f;

	//============
	//=====UI=====
	//============

protected:
	/*ui*/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		class UMaterialInterface* PrestineInventoryIcon;
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (DisplayName = "Inventory UI Class"))
		TSubclassOf<class UInventoryWidget> InventoryWidgetClass;	
	UPROPERTY()
		class UInventoryWidget* InventoryWidget;

	//==============
	//=====GRID=====
	//==============

public:
	/*the current registered Grid Key*/
	UPROPERTY()
		FIntPoint RegisteredGridKey;



	//================
	//=====EVENTS=====
	//================

public:	
	/*called whenever this inventory manager's list of items has been updated (added, removed, sorted) */
	UPROPERTY(BlueprintAssignable)
		FOnInventoryUpdated OnInventoryUpdated;

	/*called whenever this inventory manager's list of items in the vicinity has updated*/
	UPROPERTY(BlueprintAssignable)
		FOnInventoryUpdated OnInventoryItemsInVicinityUpdated;
	UPROPERTY(BlueprintAssignable)
		FOnInventoryUpdated OnInventoryManagersInVicinityUpdated;

	UPROPERTY(BlueprintAssignable)
		FOnItemEquipped OnItemEquipped;
	UPROPERTY(BlueprintAssignable)
		FOnItemEquipped OnPrimaryEquipped;
	UPROPERTY(BlueprintAssignable)
		FOnItemEquipped OnSecondaryEquipped;
	UPROPERTY(BlueprintAssignable)
		FOnItemEquipped OnAlternativeEquipped;
	UPROPERTY(BlueprintAssignable)
		FOnItemAdded OnItemAdded;
	UPROPERTY(BlueprintAssignable)
		FOnItemAdded OnItemPickedUp;
	UPROPERTY(BlueprintAssignable)
		FOnItemAdded OnPrimaryAssigned;
	UPROPERTY(BlueprintAssignable)
		FOnItemAdded OnSecondaryAssigned;
	UPROPERTY(BlueprintAssignable)
		FOnItemAdded OnAlternativeAssigned;
	UPROPERTY(BlueprintAssignable)
		FOnInventoryManagerOpened OnInventoryManagerOpened;
	UPROPERTY(BlueprintAssignable)
		FOnInventoryManagerOpened OnInventoryManagerClosed;


	//===============================================================================================================================================================================================
	//===========================================================================================FUNCTIONS===========================================================================================
	//===============================================================================================================================================================================================

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	// Sets default values for this component's properties
	UInventoryManagerComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//=====================================
	//==============INVENTORY==============
	//=====================================


protected:
	UFUNCTION()
		virtual void SpawnDefaultInventory();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void SpawnInventoryFromLoadout(class AInventoryLoadout* Loadout, bool bClearExisting);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void SpawnInventoryItems(TArray<FInventoryItemSpawnParams> Inventory);
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		virtual void SpawnInventory(TArray<TSubclassOf<class AInventoryItem>> Items, bool bClearExisting = true);

	UFUNCTION()		
		virtual class AInventoryItem* SpawnInventoryItem(TSubclassOf<AInventoryItem> ItemClass);
public:
	UFUNCTION()
		virtual void ClearInventory();
public:
	UFUNCTION(BlueprintPure, Category = "Inventory")
		class AInventoryItem* GetCurrentlyEquippedItem();

	//======================================
	//===============VICINITY===============
	//======================================

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void UpdateInventoryItemsInVicinity();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		TArray<class AInventoryItem*> GetInventoryItemsInVicinity();
	UFUNCTION(BlueprintCallable, Category = "Inventory|Vicinity")
		virtual void UpdateInventoryManagersInVicinity();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Vicinity")
		TArray<class UInventoryManagerComponent*> GetInventoryManagersInVicinity();




	//=====================================
	//===========EQUIPMENT SLOTS===========
	//=====================================



		


	//=====================================
	//==============EQUIPPING==============
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "Equipping/Unequipping")
		virtual void Equip(class AInventoryItem* Item);
protected:
	UFUNCTION()
		virtual void EquipToEquipmentSlot(class AInventoryItem* Item, class UInventoryEquipmentSlot* EquipmentSlot);

protected:
	UFUNCTION()
		virtual void OnEquipFinished(class AInventoryItem* Item);
public:
	UFUNCTION(BlueprintCallable, Category = "Equipping/Unequipping")
		virtual void Unequip(class AInventoryItem* Item);
protected:
	UFUNCTION()
		virtual void OnUnequipFinished(class AInventoryItem* Item);

	UFUNCTION()
		virtual bool ShouldAutoEquip(class AInventoryItem* Item, bool bItemPickedUp = false);

	//====================================
	//=============QUICKSLOTS=============
	//====================================

public:
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual class AInventoryItem* GetPrimaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignPrimaryWeapon(class AInventoryItem* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipPrimaryWeapon();
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual AInventoryItem* GetSecondaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignSecondaryWeapon(class AInventoryItem* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipSecondaryWeapon();
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual AInventoryItem* GetAlternativeWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignAlternativeWeapon(class AInventoryItem* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipAlternativeWeapon();

	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignToQuickslot(class AInventoryItem* ActorToAssign, EQuickslot Quickslot);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipQuickslot(EQuickslot Quickslot);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quickslots")
		virtual class AInventoryItem* GetQuickslotItem(EQuickslot Quickslot);

	//=======================================
	//============ITEM MANAGEMENT============
	//=======================================
public:	

	/*returns a copy of the inventory (in-order) - however any modifications to this will not affect the actual Inventory within the Manager for safety reasons*/
	//UFUNCTION(BlueprintPure, Category = "Inventory")
	//	const TArray<class AInventoryItem*> GetInventory() const;
	UFUNCTION(BlueprintPure, Category = "Inventory")
		const TArray<class UInventoryItemSlot*> GetInventorySlots() const;
	UFUNCTION(BlueprintPure, Category = "Inventory")
		const TArray<class UInventoryItemSlot*> GetInventorySlotsOfSize(EItemSize ItemSize);


	UFUNCTION(BlueprintPure, Category = "Inventory")
		bool ContainsItem(class AInventoryItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		bool AddItem(class AInventoryItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		bool RemoveItem(class AInventoryItem* Item);

	UFUNCTION(BlueprintPure, Category = "Interaction")
		bool CanPickup(class AInventoryItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void PickupItem(class AInventoryItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		void DropItem(class AInventoryItem* Item);



private:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
		void StoreItem(class AInventoryItem* Item);
	UFUNCTION()
		void RegisterItem(class AInventoryItem* Item);


	//===================
	//=======SLOTS=======
	//===================

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Inventory Slots")
		virtual void MoveInventorySlotToInventory(class UInventoryItemSlot* InventorySlot);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Invnetory|Inventory Slots")
		virtual bool CanInventorySlotFitInInventory(class UInventoryItemSlot* InventorySlot);
protected:
	UFUNCTION()
		virtual void AddInventorySlotToInventory(class UInventoryItemSlot* InventorySlot);
	UFUNCTION()
		virtual void RemoveInventorySlotFromInventory(class UInventoryItemSlot* InventorySlot);
	

	/*slots*/
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetMaxSlotCapacity() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetRemainingSlotCapacity() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetUsedSlotCapacity() const;

	/*item counts*/
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetMaxItemCountForSize(EItemSize Size) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetRemainingItemCountForSize(EItemSize Size) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetUsedItemCountForSize(EItemSize Size) const;

public:
	/*returns the amount of slots used by this item size*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Items")
		int32 GetItemSlotSize(EItemSize Size) const;




public:

	/*gets the slot by ID*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment Slots")
		class UInventoryEquipmentSlot* GetEquipmentSlot(FName ID);

	UFUNCTION()
		class UInventoryEquipmentSlot* CreateDefaultEquipmentSlot(FName SubobjectName, TSubclassOf<class AInventoryItem> ItemType);


	/*returns the first equipment slot matching the template*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment Slots")
		class UInventoryEquipmentSlot* GetEquipmentSlotByClass(TSubclassOf<AInventoryItem> ItemClass);
	template <typename T>
	class UInventoryEquipmentSlot* GetEquipmentSlot() const
	{
		return GetEquipmentSlotByClass(T::StaticClass());
	}




protected:	
	class UInventoryItemSlot* CreateInventoryItemSlot(FInventoryItemSpawnParams ItemSpawnParams, bool bSpawnInstance = false);
	class UInventoryItemSlot* CreateInventorySlot(TSubclassOf<class AInventoryItem> ItemClass);
	




protected:
	UFUNCTION()
		virtual void NativeOnInventoryUpdated();

	//==============================
	//=============GRID=============
	//==============================


public:
	UFUNCTION()
		void UpdateGridKey(FIntPoint NewGridKey);


public:
	UFUNCTION(BlueprintPure, Category = "Inventory|Items")
		virtual UInventoryItemComponent* GetItemComponent(class AInventoryItem* Item);


	//==============================
	//==============UI==============
	//==============================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializeInventoryWidget(class APlayerController* Controller);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		FText GetInventoryManagerName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		class UMaterialInterface* GetInventoryIcon();

	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void OpenInventoryWidget();
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void CloseInventoryWidget();
	UFUNCTION(BlueprintPure, Category = "UI", meta = (DisplayName = "Is Inventory UI Open"))
		virtual bool IsInventoryWidgetOpen();


	//==================================
	//===========REGISTRATION===========
	//==================================
	private:
		/*registers itself with the Inventory Subsystem*/
		UFUNCTION()
			virtual void RegisterInventoryManager();
};
