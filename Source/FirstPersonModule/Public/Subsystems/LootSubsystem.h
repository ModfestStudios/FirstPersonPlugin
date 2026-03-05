// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LootSubsystem.generated.h"


/*structure that holds a list of items or managers within a specific section of the map*/
USTRUCT()
struct FInventoryGridData
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<class UInventoryItemComponent*> Items;
	UPROPERTY()
		TArray<class UInventoryManagerComponent*> Managers;
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ULootSubsystem : public UTickableWorldSubsystem
{
	friend class UInventoryManagerComponent;
	friend class UInventoryItemComponent;

	GENERATED_BODY()
public:

	/*list of items and managers split chunked up by their location in physical space (determined by GridChunkSize)*/
	UPROPERTY()
		TMap<FIntPoint, FInventoryGridData> InventoryGrid;	


	/*sized used to group inventory/inventory managers for more optimized checks*/
	static constexpr int GridChunkSize = 10000;

	//=====================================================================================================================
	//======================================================FUNCTIONS======================================================
	//=====================================================================================================================
	

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(ULootSubsystem, STATGROUP_Tickables); }

protected:
	UFUNCTION()
		void RegisterDroppedItem(class UInventoryItemComponent* ItemComponent);
	UFUNCTION()
		void UnregisterDroppedItem(class UInventoryItemComponent* ItemComponent, FIntPoint GridKey);
	UFUNCTION()
		void RegisterInventoryManager(class UInventoryManagerComponent* InventoryManager);
	UFUNCTION()
		void UnregisterInventoryManager(class UInventoryManagerComponent* InventoryManager, FIntPoint GridKey);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loot Subsystem")
		const TArray<UInventoryItemComponent*> GetItemsInVicinty(const FVector& Location, const float Threshold = 1000.0f);
	
	/*checks nearby grids if they're close*/
	UFUNCTION()
		bool IsGridWithinThreshold(const FVector& Location, const FIntPoint& GridKey, float ThresholdSquared) const;


	/*returns a converted grid key for looking up inventory items based on grid space*/
	UFUNCTION(BlueprintCallable, Category = "Loot Subsystem")
		FIntPoint GetGridKey(const FVector& Location) const;
};
