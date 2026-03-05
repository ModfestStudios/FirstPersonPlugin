// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/InventoryItem.h"
#include "Interfaces/InventoryItemInterface.h"
#include "InventoryItemAsset.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory Item")
		FText Name;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory Item", meta=(MustImplement = InventoryItemInterface))
		TSoftClassPtr<class AActor> ItemClass;



	

	
};
