// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
//#include "Inventory/InventoryItem.h"
#include "../Interfaces/PrimaryWeaponInterface.h"
#include "../Interfaces/SecondaryWeaponInterface.h"
#include "../Interfaces/AlternativeWeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

static const uint8 INVENTORY_SLOT_SIZE_SM = 1;
static const uint8 INVENTORY_SLOT_SIZE_MD = 2;
static const uint8 INVENTORY_SLOT_SIZE_LG = 4;
static const uint8 INVENTORY_SLOT_SIZE_XLG = 8;

USTRUCT(BlueprintType)
struct FInventoryGridEntryDebugData
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		FString Name;
	UPROPERTY(BlueprintReadOnly)
		FVector Location;
	UPROPERTY(BlueprintReadOnly)
		UClass* Class;
	UPROPERTY(BlueprintReadOnly)
		float Distance;


	FInventoryGridEntryDebugData()
	{
		Name = "Unknown";
		Location = FVector(0);
		Class = nullptr;
		Distance = 0.0f;
	}

};

USTRUCT(BlueprintType)
struct FInventoryGridDebugData
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		FIntPoint GridKey;
	UPROPERTY(BlueprintReadOnly)
		TArray<FInventoryGridEntryDebugData> Entries;

	FInventoryGridDebugData() {}
};


UENUM(BlueprintType)
enum class EInventoryEquipmentSlotKind : uint8
{
    Equipment,
    MainHand,
    OffHand
};

/** Editor configuration only. Slot components are created at runtime by the manager. */
USTRUCT(BlueprintType)
struct FInventoryEquipmentSlotConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    FName SlotID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    EInventoryEquipmentSlotKind SlotKind = EInventoryEquipmentSlotKind::Equipment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", meta = (AllowAbstract = "true"))
    TSubclassOf<class AInventoryItem> AcceptedItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", meta = (DisplayName = "Default Item Class to Spawn", ToolTip = "Inventory item actor class spawned during default inventory initialization at startup. The manager tries to equip it into this slot, then falls back to storage. Leave empty to start without a default item."))
    TSubclassOf<class AInventoryItem> DefaultItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    FName AttachmentSocket;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    FName AttachmentComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    FTransform AttachmentTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
    int32 StoragePriority = 0;
};

UENUM(BlueprintType)
enum class EItemPresence : uint8
{
	World,
	Storage,
	Attached
};

UENUM(BlueprintType)
enum class EInventoryView : uint8
{
	None,
	Vicinity,
	Inventory
};

USTRUCT(BlueprintType)
struct FInventoryManagerVicinityCheckParameters
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadWrite, meta = (ToolTip = "Actors explicitly excluded from nearby inventory results."))
		TArray<AActor*> IgnoredActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Also ignore actors equipped in the supplied IgnoredActors inventory equipment slots, recursively through nested equipment. Does not automatically choose an actor to ignore."))
		bool bAutoIgnoreRecursive = false;

	FInventoryManagerVicinityCheckParameters() {}
};

/*structure that holds a list of items or managers within a specific section of the map*/
USTRUCT()
struct FInventoryGridEntry
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<TWeakObjectPtr<class AInventoryItem>> InventoryItems;
	UPROPERTY()
		TArray<TWeakObjectPtr<class UInventoryManagerComponent>> Managers;
};

/** Terminal result of a pickup, drop, equipping, or unequipping operation. */
UENUM(BlueprintType)
enum class EInventoryItemOperationEndReason : uint8
{
	Completed,
	Canceled,
	Interrupted
};

UENUM(BlueprintType)
enum class EEquipCompletionLogic : uint8
{
	Instant,
	Delayed,
	WaitForNotification
};

UENUM(BlueprintType)
enum class EEquipState : uint8 {
	NONE,
	Equipping,
	Unequipping,
	Equipped
};

UENUM(BLueprintType)
enum class EItemCondition : uint8
{	Prestine,
	Worn,
	Damaged,	
	BadlyDamaged,
	Ruined
};

UENUM(BlueprintType)
enum class EItemSize : uint8
{
	Small,
	Medium,
	Large,
	ExtraLarge
};


UENUM(BlueprintType)
enum class EQuickslot : uint8
{
	Quickslot1,
	Quickslot2,
	Quickslot3,
	Quickslot4,
	Quickslot5,
	Quickslot6,
	Quickslot7,
	Quickslot8,
	Quickslot9,
	Quickslot10
};


USTRUCT(BlueprintType)
struct FInventoryItemSpawnParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
		FString DisplayName;
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TSubclassOf<class AInventoryItem> ItemClass;
	UPROPERTY(EditAnywhere, Category = "Inventory")
		EItemCondition Condition = EItemCondition::Prestine;
	UPROPERTY(EditAnywhere, Category = "Inventory")
		int32 Quantity = 1;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		bool bEquipOnSpawn = false;

	//attachments?
	//TArray<FInventoryItemSpawnParams> Attachments;

	//bool MatchesItem(const TSubclassOf<class AActor> Item)
	//{
	//	return ItemClass == Item;
	//}
};



