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
	friend class AInventoryItem;


	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details")
		FText ItemName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details")
		FText ItemDescription;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details|Icons")
		class UMaterialInterface* PrestineItemIcon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details|Icons")
		class UMaterialInterface* WornItemIcon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details|Icons")
		class UMaterialInterface* DamagedItemIcon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details|Icons")
		class UMaterialInterface* BadlyDamageItemIcon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Item|Item Details|Icons")
		class UMaterialInterface* RuinedItemIcon;




public:
	UPROPERTY(EditAnywhere, Category = "Inventory Item")
		EItemSize Size;
	UPROPERTY(EditAnywhere, Category = "Inventory Item")
		EItemCondition Condition = EItemCondition::Prestine;

protected:
	/*if enabled - allows item to be used with a Point of Sale system*/
	UPROPERTY(EditAnywhere, Category = "Inventory Item|Point of Sale")
		bool bSellableItem = false;
	UPROPERTY(EditAnywhere, Category = "Inventory Item|Point of Sale", meta = (EditCondition = "bSellableItem", EditConditionHides))
		float MSRP = 9.99f;


	/*reference to inventory manager when assigned*/
private:
	UPROPERTY(ReplicatedUsing=OnRep_ItemPresence)
        class UInventoryManagerComponent* InventoryManager = nullptr;
    UPROPERTY()
        TWeakObjectPtr<class UInventoryManagerComponent> PreviousStorage;
    UPROPERTY(ReplicatedUsing=OnRep_ItemPresence)
        class UInventoryAttachmentComponent* AttachmentSlot = nullptr;
public:
	/*automatically equips this item when picked up or spawned into inventory*/
	UPROPERTY(EditAnywhere, Category = "Equipping/Unequipping")
		bool bAutoEquip = false;
    UPROPERTY(EditAnywhere, Category="Equipping/Unequipping")
        bool bCompleteEquipAutomatically = true;
	/*if called - will immedietely unhide the mesh. Otherwise it's up to the actor itself to unhide when needed*/
	UPROPERTY(EditAnywhere, Category = "Equipping/Unequipping")
		bool bAutoUnhideOnEquip = true;
	/*if the Inventory Manager already has a currently equipped item - will check to see which priority is higher to swap*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipping/Unequipping")
		uint8 EquipPriority = 0;
private:
	/**/
	UPROPERTY(ReplicatedUsing=OnRep_ItemPresence)
		EEquipState EquipState = EEquipState::NONE;

	EItemPresence LastNotifiedPresence = EItemPresence::World;
    TWeakObjectPtr<AActor> LastNotifiedHolder;
    UPROPERTY(Transient)
        TWeakObjectPtr<class AFirstPersonCharacter> EquipRequestUser;
    FTimerHandle EquipHandler;
	FTimerHandle UnequipHandler;
	
public:
	/*the current registered Grid Key*/
	UPROPERTY()
		FIntPoint RegisteredGridKey;

private:
	UPROPERTY(ReplicatedUsing=OnRep_ItemPresence)
		EItemPresence ItemPresence = EItemPresence::World;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Sets default values for this component's properties
	UInventoryItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	//=================================
	//=============DETAILS=============
	//=================================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item|Details")
		FText GetItemName() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item|Details")
		FText GetItemDescription() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item|Details")
		class UMaterialInterface* GetItemIcon() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Item|Details")
		EItemSize GetItemSize() const;

	
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
	UFUNCTION(BlueprintPure, Category = "Inventory Item|State")
		EEquipState GetEquipState() { return EquipState; };
	UFUNCTION(BlueprintPure, Category = "Inventory Item|State")
		EItemPresence GetItemPresence();

private:
	UFUNCTION()
		void SetItemPresence(EItemPresence NewPresence);
    UFUNCTION()
        void OnRep_ItemPresence();

	


	//=================================
	//============EQUIPPING============
	//=================================
public:
	/*this typically is called by the InventoryManager & routes down to the owning Actor*/
	UFUNCTION()
		virtual void BeginEquip(class AFirstPersonCharacter* User = nullptr);
	/*EndEquip() then gets called back from the owning actor after it's done its equip logic to finalize the states of the ItemComp*/
	UFUNCTION()
		virtual void EndEquip();
	UFUNCTION()
		virtual void BeginUnequip();
	UFUNCTION()
		virtual void EndUnequip();


	//===============================
	//============STORAGE============
	//===============================
private:
	//UFUNCTION()
	//	virtual void NativeOnStoredInsideInventoryManager(class UInventoryManagerComponent* InInventoryManager);



public:
	UFUNCTION(BlueprintPure, Category = "Inventory Manager")
		class UInventoryManagerComponent* GetInventoryManager() { return InventoryManager; };
    UFUNCTION(BlueprintPure, Category="Inventory Manager")
        UInventoryManagerComponent* GetPreviousStorage() const { return PreviousStorage.Get(); }
    UFUNCTION(BlueprintPure, Category="Inventory Manager")
        UInventoryAttachmentComponent* GetAttachmentSlot() const { return AttachmentSlot; }
	UFUNCTION(BlueprintPure, Category = "Inventory Manager")
		class AActor* GetOwningActor();

public:
	UFUNCTION()
		void UpdateGridKey(FIntPoint NewGridKey);
	UFUNCTION()
		virtual void RegisterInventoryItem();
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

	//==================
	//====PREDICATES====
	//==================
	public:

		static bool SortBySize(const UInventoryItemComponent& ItemA, const UInventoryItemComponent& ItemB)
		{
			return ItemA.Size < ItemB.Size;
		}
};
