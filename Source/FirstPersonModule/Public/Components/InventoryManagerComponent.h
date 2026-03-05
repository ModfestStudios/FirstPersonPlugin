// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Components/ActorComponent.h"
#include "InventoryManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, UInventoryManagerComponent*, InventoryManager, AActor*, OwningActor, AActor*, InventoryItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, AActor*, Item);


UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent), HideCategories = (Sockets, ComponentTick, ComponentReplication, Activation, Cooking, Collision, AssetUserData))
class FIRSTPERSONMODULE_API UInventoryManagerComponent : public UActorComponent
{
	friend class UInventoryItemComponent;

	GENERATED_BODY()
public:


protected:
	/*what's currently active for this manager*/
	UPROPERTY(Replicated)
		AActor* CurrentlyEquipped;
	UPROPERTY(Replicated)
		AActor* PrimaryWeapon;
	UPROPERTY(Replicated)
		AActor* AlternativeWeapon;
	UPROPERTY(Replicated)
		AActor* SecondaryWeapon;
private:
	/*used to track an item that wants to be equipped but waiting on an action to complete*/
	UPROPERTY()
		AActor* PendingEquip;
protected:
	/*Primary Weapon - must implement PrimaryWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnablePrimaryWeapon", EditConditionHides, MustImplement = "PrimaryWeaponInterface", DisplayName = "Primary Weapon"))
		TSubclassOf<AActor> DefaultPrimary;
	/*Primary Weapon - must implement SecondaryWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnableSecondaryWeapon", EditConditionHides, MustImplement = "SecondaryWeaponInterface", DisplayName = "Secondary Weapon"))
		TSubclassOf<AActor> DefaultSecondary;
	/*Primary Weapon - must implement AlternativeWeaponInterface*/
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bEnableAlternativeWeapon", EditConditionHides, MustImplement = "AlternativeWeaponInterface", DisplayName = "Alternative Weapon"))
		TSubclassOf<AActor> DefaultAlternative;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (MustImplement = "InventoryItemInterface", DisplayName = "Inventory"))
		TArray<TSubclassOf<AActor>> DefaultItems;


protected:
	UPROPERTY(Replicated)
		TArray<AActor*> Inventory;

	UPROPERTY()
		TArray<AActor*> Quickslots;


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


	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Tracing")
		float MaxVicinityItemDistance = 250.0f;

protected:
	/*ui*/
	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (DisplayName = "Inventory UI Class"))
		TSubclassOf<class UInventoryWidget> InventoryWidgetClass;	
	UPROPERTY()
		class UInventoryWidget* InventoryWidget;

public:
	/*the current registered Grid Key*/
	UPROPERTY()
		FIntPoint RegisteredGridKey;

public:
	/*event/delegates*/
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


	//===================================================================================================
	//=============================================FUNCTIONS=============================================
	//===================================================================================================

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//=====================================
	//==============INVENTORY==============
	//=====================================


protected:
	UFUNCTION()
		virtual void SpawnDefaultInventory();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void SpawnInventoryFromLoadout(class AInventoryLoadout* Loadout, bool bClearExisting);
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		virtual void SpawnInventory(TArray<TSubclassOf<AActor>> Items, bool bClearExisting = true);
	UFUNCTION()
		virtual AActor* SpawnInventoryItem(TSubclassOf<AActor> ItemClass);
public:
	UFUNCTION()
		virtual void ClearInventory();
public:
	UFUNCTION(BlueprintPure, Category = "Inventory")
		AActor* GetCurrentlyEquippedItem();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
		TArray<class UInventoryItemComponent*> GetItemsInVicinity();


	//=====================================
	//==============EQUIPPING==============
	//=====================================
public:
	UFUNCTION(BlueprintCallable, Category = "Equipping/Unequipping")
		virtual void Equip(AActor* Item);
protected:
	UFUNCTION()
		virtual void OnEquipFinished(AActor* Item);
public:
	UFUNCTION(BlueprintCallable, Category = "Equipping/Unequipping")
		virtual void Unequip(AActor* Item);
protected:
	UFUNCTION()
		virtual void OnUnequipFinished(AActor* Item);

	UFUNCTION()
		virtual bool ShouldAutoEquip(AActor* Item, bool bItemPickedUp = false);

	//====================================
	//=============QUICKSLOTS=============
	//====================================

public:
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual AActor* GetPrimaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignPrimaryWeapon(AActor* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipPrimaryWeapon();
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual AActor* GetSecondaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignSecondaryWeapon(AActor* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipSecondaryWeapon();
	UFUNCTION(BlueprintPure, Category = "Quickslots")
		virtual AActor* GetAlternativeWeapon();
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignAlternativeWeapon(AActor* Weapon);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipAlternativeWeapon();

	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void AssignToQuickslot(AActor* ActorToAssign, EQuickslot Quickslot);
	UFUNCTION(BlueprintCallable, Category = "Quickslots")
		virtual void EquipQuickslot(EQuickslot Quickslot);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quickslots")
		virtual AActor* GetQuickslotItem(EQuickslot Quickslot);

	//=======================================
	//============ITEM MANAGEMENT============
	//=======================================
public:
	/*returns a copy of the inventory (in-order) - however any modifications to this will not affect the actual Inventory within the Manager for safety reasons*/
	UFUNCTION(BlueprintPure, Category = "Inventory")
		TArray<AActor*> GetInventory() { return Inventory; };
	UFUNCTION(BlueprintPure, Category = "Inventory")
		bool ContainsItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		bool AddItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		bool RemoveItem(AActor* Item);

	UFUNCTION(BlueprintPure, Category = "Interaction")
		bool CanPickup(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void PickupItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory|Item Management")
		void DropItem(AActor* Item);

private:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Items")
		void StoreItem(AActor* Item);
	UFUNCTION()
		void RegisterItem(AActor* Item);

public:
	UFUNCTION()
		void UpdateGridKey(FIntPoint NewGridKey);


public:
	UFUNCTION(BlueprintPure, Category = "Inventory|Items")
		virtual UInventoryItemComponent* GetItemComponent(AActor* Item);


	//==============================
	//==============UI==============
	//==============================
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void InitializeInventoryWidget(class APlayerController* Controller);

	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void OpenInventoryUI();
	UFUNCTION(BlueprintCallable, Category = "UI")
		virtual void CloseInventoryUI();
	UFUNCTION(BlueprintPure, Category = "UI", meta = (DisplayName = "Is Inventory UI Open"))
		virtual bool IsInventoryUIOpen();
};
