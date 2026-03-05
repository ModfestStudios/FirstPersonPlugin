// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ULoadoutWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
		class AInventoryLoadout* Loadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loadout", meta = (MustImplement = "PrimaryWeaponInterface"))
		TArray<TSubclassOf<AActor>> PrimaryWeaponsList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loadout", meta = (MustImplement = "SecondaryWeaponInterface"))
		TArray<TSubclassOf<AActor>> SecondaryWeaponsList;

	

	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================
	

	/*will spawn a completely empty loadout*/
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		virtual void CreateNewLoadout();



};
