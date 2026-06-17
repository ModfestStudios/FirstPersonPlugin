// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryManagerEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryManagerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Manager Entry")
		class UInventoryManagerComponent* InventoryManager;


	//=======================================================================================================================================================================
	//===============================================================================FUNCITONS===============================================================================
	//=======================================================================================================================================================================

	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventroy Manager Entry")
		FText GetInventoryManagerName();	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventroy Manager Entry")
		class UMaterialInterface* GetInventoryIcon();


	
};
