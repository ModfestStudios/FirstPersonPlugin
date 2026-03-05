// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionWeather.generated.h"

/**
 * 
 */
UCLASS(collapsecategories, hidecategories = Object)
class FIRSTPERSONMODULE_API UMaterialExpressionWeather : public UMaterialExpression
{
	GENERATED_BODY()
public:

	UPROPERTY()
		FExpressionInput Rain;
	UPROPERTY()
		FExpressionInput Snow;


#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;

	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual bool NeedsRealtimePreview() override { return true; }
#endif

	
};
