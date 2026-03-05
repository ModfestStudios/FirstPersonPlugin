// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Firearm.h"
#include "Inventory/Inventory.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AShotgun : public AFirearm, public IPrimaryWeaponInterface
{
	GENERATED_BODY()
	
};
