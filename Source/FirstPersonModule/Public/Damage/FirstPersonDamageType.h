// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "FirstPersonDamageType.generated.h"

enum class EGibCategory : uint8;

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UFirstPersonDamageType : public UDamageType
{
	GENERATED_BODY()
public:


	/*final blow must have at least this amount of damage to be considered for gore*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gore")
		float GibbingThreshold = 200.0f;
	/*the Gore system will use this to determine what should happen to the player's mesh*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type")
		EGibCategory GoreCategory;
};
