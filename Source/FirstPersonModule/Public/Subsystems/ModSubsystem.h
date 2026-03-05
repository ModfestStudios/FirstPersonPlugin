// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maps/MapAsset.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ModSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UModSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

private:
	UPROPERTY()
		TArray<UMapAsset*> Maps;



	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================
	

public:
	UFUNCTION(BlueprintCallable, Category = "Maps & Missions")
		void AddMap(class UMapAsset* NewMap);
	UFUNCTION(BlueprintCallable, Category = "Maps & Missions")
		TArray<UMapAsset*> GetAllMaps();
	UFUNCTION(BlueprintCallable, Category = "Maps & Missions")
		TArray<UMapAsset*> GetMapsByCategory(const FName Category);
	UFUNCTION(BlueprintCallable, Category = "Maps & Missions")
		TArray<UMapAsset*> GetMapsByTag(const FName Tag, const FName Value);

	UFUNCTION(BlueprintCallable, Category = "Maps & Missions|Utilities")
		void SortMapListAlphabetically(TArray<UMapAsset*>& MapList);



};
