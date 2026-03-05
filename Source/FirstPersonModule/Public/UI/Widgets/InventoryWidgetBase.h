// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidgetBase.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget|Vicinity Checking")
		bool bCheckForItemsInVicinity = false;
	/*in seconds - how often to check for new items around us*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget|Vicinity Checking")
		float ItemCheckRate;
	/*set the grid panel inside your UMG here to auto-update it*/
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget|Vicinity Checking", meta=(BindWidgetOptional))
		class UPanelWidget* VicinityGridWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget|Vicinity Checking")
		TSubclassOf<class UInventorySlotEntry> InventorySlotClass;
	FTimerHandle ItemVicinityHandler;

	

	//===============================================================================================================================================
	//===================================================================FUNCTIONS===================================================================
	//===============================================================================================================================================
	
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct();

	virtual void SynchronizeProperties() override;

	UFUNCTION()
		virtual void InitializeItemVicinityCheck();

	UFUNCTION()
		void UpdateItemsInVicinityList();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Widget")
		TArray<class UInventoryItemComponent*> GetItemsInVicinity();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Widget")
		class AFirstPersonCharacter* GetOwningCharacter();

	
};
