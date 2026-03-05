// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MapAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONEDITORMODULE_API UMapAssetFactory : public UFactory
{
	GENERATED_BODY()
public:

	UMapAssetFactory();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FText GetDisplayName() const;
	virtual uint32 GetMenuCategories() const;
	virtual FText GetToolTip() const;
	
};
