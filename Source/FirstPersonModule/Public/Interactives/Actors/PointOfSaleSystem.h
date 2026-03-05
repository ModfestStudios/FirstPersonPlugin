// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactives/Actors/TerminalSystem.h"
#include "PointOfSaleSystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API APointOfSaleSystem : public ATerminalSystem
{
	GENERATED_BODY()
public:


public:
	UPROPERTY(BlueprintReadOnly, Category = "POS|Sales")
		TArray<class AActor*> ScannedItems;




	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================


public:
	/*returns FALSE if item is unsellable or missing InventoryItemComponent*/
	UFUNCTION(BlueprintCallable, Category = "POS|Sales")
		virtual bool ScanItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "POS|Sales")
		virtual bool RemoveItem(AActor* Item);



	
};
