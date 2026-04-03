// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maps/MapAsset.h"
#include "Subsystems/MissionSubsystem.h"
#include "MissionAsset.generated.h"



/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API UMissionAsset : public UMapAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		FText MissionName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		EMissionType MissionType = EMissionType::Assault;

	static const FPrimaryAssetType PrimaryAssetType;

	//=================================================================================================
	//============================================FUNCTIONS============================================
	//=================================================================================================

	UMissionAsset();


	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(PrimaryAssetType, GetFName());
	}

	static bool SortAlphabetically(const UMissionAsset& ItemA, const UMissionAsset& ItemB)
	{
		return ItemA.MissionName.ToString() < ItemB.MissionName.ToString();
	}

};
