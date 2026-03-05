// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InventorySubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:



private:
	UPROPERTY()
		class UInventoryItemAsset* HoveredItemAsset;

	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================


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
	
};
