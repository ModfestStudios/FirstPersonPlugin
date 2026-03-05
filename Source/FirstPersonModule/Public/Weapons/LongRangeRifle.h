// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Weapons/Firearm.h"
#include "LongRangeRifle.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API ALongRangeRifle : public AFirearm, public IPrimaryWeaponInterface
{
	GENERATED_BODY()
	
};
