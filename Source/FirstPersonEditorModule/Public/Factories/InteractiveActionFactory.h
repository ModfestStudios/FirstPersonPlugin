// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InteractiveActionFactory.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONEDITORMODULE_API UInteractiveActionFactory : public UFactory
{
	GENERATED_BODY()
public:


	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================
	UInteractiveActionFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FText GetDisplayName() const;
	virtual uint32 GetMenuCategories() const;
	virtual FText GetToolTip() const;
	
};
