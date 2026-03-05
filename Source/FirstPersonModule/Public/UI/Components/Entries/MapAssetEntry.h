// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MapAssetEntry.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapAssetEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	//bool bHoverState = false;

	UPROPERTY(EditAnywhere, Category = "Map Selector")
		bool bAddToMapRotationOnSelection = false;
	UPROPERTY(BlueprintReadOnly, Category = "Map Info")
		class UMapAsset* MapInfo;

	/*events*/
	/*UPROPERTY(BlueprintAssignable)
		FHoverStateChange OnHover;
	UPROPERTY(BlueprintAssignable)
		FHoverStateChange OnUnhover;*/

	//===================================================================================================================
	//=====================================================FUNCTIONS=====================================================
	//===================================================================================================================

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;


	UFUNCTION(BlueprintCallable, Category = "Map Selection")
		virtual void SetAsNextMap();
	UFUNCTION(BlueprintCallable, Category = "Map Selection")
		virtual void AddToMapRotation();
	
};
