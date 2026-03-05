// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maps/MapAsset.h"
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


	static const FPrimaryAssetType PrimaryAssetType;

	//=================================================================================================
	//============================================FUNCTIONS============================================
	//=================================================================================================


	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(PrimaryAssetType, GetFName());
	}

};
