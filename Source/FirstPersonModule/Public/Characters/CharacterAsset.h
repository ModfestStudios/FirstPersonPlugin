// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Engine/DataAsset.h"
#include "CharacterAsset.generated.h"


/** UCharacterAsset - This class is meant to allow someone to create a "CharacterInfo" as an Content Browser asset, to be used when a GameMode may have
 * pre-defined Characters to select from for example. It's simply a Content Browser friendly asset to easily create default characters.
 */
UCLASS()
class FIRSTPERSONMODULE_API UCharacterAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Core")
		FString FirstName;
	UPROPERTY(EditAnywhere, Category = "Core")
		FString LastName;



	/*inventory*/	
	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
		FInventoryItemSpawnParams PrimaryWeapon;
	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
		FInventoryItemSpawnParams SecondaryWeapon;

};
