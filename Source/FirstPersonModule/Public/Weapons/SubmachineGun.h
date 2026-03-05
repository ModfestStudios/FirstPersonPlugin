// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Firearm.h"
#include "Inventory/Inventory.h"
#include "SubmachineGun.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API ASubmachineGun : public AFirearm, public IPrimaryWeaponInterface,public IAlternativeWeaponInterface
{
	GENERATED_BODY()
	
};
