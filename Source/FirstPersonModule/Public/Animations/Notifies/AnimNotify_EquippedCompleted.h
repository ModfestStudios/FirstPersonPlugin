// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EquippedCompleted.generated.h"

/**
 * 
 */
UCLASS(const, HideCategories = Object, CollapseCategories, meta = (DisplayName = "Equip - Completed"))
class FIRSTPERSONMODULE_API UAnimNotify_EquippedCompleted : public UAnimNotify
{
	GENERATED_BODY()
public:


	virtual FString GetNotifyName_Implementation() const override { return "Item Equip - Completed"; };
};
