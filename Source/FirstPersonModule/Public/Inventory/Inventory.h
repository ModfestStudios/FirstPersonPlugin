// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "../Interfaces/InventoryItemInterface.h"
#include "../Interfaces/PrimaryWeaponInterface.h"
#include "../Interfaces/SecondaryWeaponInterface.h"
#include "../Interfaces/AlternativeWeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

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
		TSubclassOf<class AActor> ItemClass;
	UPROPERTY(EditAnywhere, Category = "Inventory")
		EItemCondition Condition = EItemCondition::Prestine;
	UPROPERTY(EditAnywhere, Category = "Inventory")
		int32 Quantity = 1;
	//attachments?
	//TArray<FInventoryItemSpawnParams> Attachments;

	bool MatchesItem(const TSubclassOf<class AActor> Item)
	{
		return ItemClass == Item;
	}
};



