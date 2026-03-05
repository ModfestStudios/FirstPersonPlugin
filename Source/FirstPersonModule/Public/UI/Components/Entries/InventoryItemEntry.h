// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryItemEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryItemEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		class AActor* ItemActor;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
		class UInventoryItemComponent* ItemComponent;


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================


	/*integration support for List UMG Widgets that automatically calls SetCharacterInfo() for us*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	
};
