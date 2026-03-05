// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryItemAssetEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemAssetEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, Category = "Inventory Asset")
		class UInventoryItemAsset* ItemAsset;

	bool bHoverState = false;

	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:
	UFUNCTION()
		virtual void Hover();
	UFUNCTION()
		virtual void Unhover();


	
};
