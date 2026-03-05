// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MapInfoEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHoverStateChange);


/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UMapInfoEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	bool bHoverState = false;

	UPROPERTY(EditAnywhere, Category = "Map Selector")
		bool bAddToMapRotationOnSelection = false;
	UPROPERTY(BlueprintReadOnly, Category = "Map Info")
		class UMapAsset* MapInfo;

	/*events*/
	UPROPERTY(BlueprintAssignable)
		FHoverStateChange OnHover;
	UPROPERTY(BlueprintAssignable)
		FHoverStateChange OnUnhover;

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

	UFUNCTION(BlueprintCallable, Category = "Hover")
		virtual void NotifyOfHover();
	UFUNCTION(BlueprintImplementableEvent, Category = "Hover", meta=(DisplayName="OnHover"))
		void BP_OnHover();
	
	UFUNCTION(BlueprintCallable, Category = "Hover")
		virtual void NotifyOfUnhover();
	UFUNCTION(BlueprintImplementableEvent, Category = "Hover",meta = (DisplayName = "OnUnhover"))
		void BP_OnUnhover();




	
};
