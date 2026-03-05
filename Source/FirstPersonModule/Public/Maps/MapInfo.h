// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MapInfo.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AMapInfo : public AInfo
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Region")
		FText Region;


	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Tags")
	//	TMap<FName, FName> Tags;



	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================s
	

	static bool SortAlphabetically(const AMapInfo& ItemA, const AMapInfo& ItemB)
	{
		return ItemA.MapName.ToString() < ItemB.MapName.ToString();
	}

};
