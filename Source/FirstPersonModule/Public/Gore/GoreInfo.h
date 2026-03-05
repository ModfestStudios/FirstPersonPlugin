// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GoreInfo.generated.h"

enum class EGibCategory : uint8;

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UGoreInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	
	/*when the gib system is called - it will match the category that's passed in*/
	UPROPERTY(EditDefaultsOnly, Category = "Gibs")
		EGibCategory GoreCategory;



};
