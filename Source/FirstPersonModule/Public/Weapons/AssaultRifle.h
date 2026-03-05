// Copyright Modfest Studios

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "Weapons/Firearm.h"
#include "AssaultRifle.generated.h"

/**
 * 
 */
UCLASS(abstract)
class FIRSTPERSONMODULE_API AAssaultRifle : public AFirearm, public IPrimaryWeaponInterface
{
	GENERATED_BODY()
public:


	//===========================================================================================================
	//=================================================FUNCTIONS=================================================
	//===========================================================================================================
	AAssaultRifle(const FObjectInitializer& ObjectIntializer);
	
};
