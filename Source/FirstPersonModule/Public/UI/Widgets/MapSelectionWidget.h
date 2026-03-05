// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Map Selection")
		TArray<class UMapAsset*> Maps;
	
	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================

public:
	/*grabs */
	UFUNCTION(BlueprintCallable, Category = "Map Selection")
		void InitMapListByCategory(const FName Category);

};
