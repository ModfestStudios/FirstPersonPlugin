// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemList : public UUserWidget
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Items", meta = (MustImplement = "InventoryItemInterface"))
		TArray<TSubclassOf<AActor>> ItemList;

	/*name your List View widget in your Blueprint this name for it to auto-bind*/
	UPROPERTY(VisibleDefaultsOnly, Category = "Item List")
		FName ListViewWidgetName = "List View";
	UPROPERTY(VisibleDefaultsOnly, Category = "Item List", meta = (BindWidget))
		class UListView* ListView;

	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================


	virtual void NativeConstruct() override;


	UFUNCTION(BlueprintCallable, Category = "Inventory Items")
		virtual void RefreshItemList();




};
