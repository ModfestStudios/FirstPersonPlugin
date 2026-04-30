// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
//#include "Inventory/InventoryItem.h"
#include "../Interfaces/InventoryItemInterface.h"
#include "../Interfaces/PrimaryWeaponInterface.h"
#include "../Interfaces/SecondaryWeaponInterface.h"
#include "../Interfaces/AlternativeWeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

static const uint8 INVENTORY_SLOT_SIZE_SM = 1;
static const uint8 INVENTORY_SLOT_SIZE_MD = 2;
static const uint8 INVENTORY_SLOT_SIZE_LG = 4;
static const uint8 INVENTORY_SLOT_SIZE_XLG = 8;

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


	UPROPERTY()
		TArray<AActor*> IgnoredActors;

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

UENUM(BlueprintType)
enum class EItemState : uint8 {
	NONE,
	AsPickup,
	InStorage,
	InUse
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



