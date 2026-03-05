// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API UMapAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		FText MapName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		FString Author;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map", meta=(Multiline = "true"))
		FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		FName Category;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
		TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map Selection")
		class UMaterialInterface* PosterImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map Selection")
		class UMaterialInterface* LoadingScreenImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Region")
		FText Region;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Tags")
		TMap<FName, FName> Tags;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|meta")
		uint8 MinPlayersRecommended;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|meta")
		uint8 MaxPlayersRecommended;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|meta")
		float ExpectedPlayTime;

	static const FPrimaryAssetType PrimaryAssetType;


	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================s
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(PrimaryAssetType, GetFName());
	}

	static bool SortAlphabetically(const UMapAsset& ItemA, const UMapAsset& ItemB)
	{
		return ItemA.MapName.ToString() < ItemB.MapName.ToString();
	}

};
