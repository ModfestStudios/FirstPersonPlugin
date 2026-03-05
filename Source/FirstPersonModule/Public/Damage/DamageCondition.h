// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DamageCondition.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UDamageCondition : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
		FName Name;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
		FText Description;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General", meta = (ClampMax = "1.0", ClampMin = "0.0"))
		float Intensity;



	//===========================================================================
	//=============================CONDITIONAL SETUP=============================
	//===========================================================================

	
	
};
