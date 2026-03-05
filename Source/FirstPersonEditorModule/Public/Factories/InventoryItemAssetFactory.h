// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InventoryItemAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONEDITORMODULE_API UInventoryItemAssetFactory : public UFactory
{
	GENERATED_BODY()
public:

	UInventoryItemAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FText GetDisplayName() const;
	virtual uint32 GetMenuCategories() const;
	virtual FText GetToolTip() const;

	
};
