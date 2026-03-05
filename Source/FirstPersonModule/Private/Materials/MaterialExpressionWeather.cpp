// Fill out your copyright notice in the Description page of Project Settings.


#include "Materials/MaterialExpressionWeather.h"

#if WITH_EDITOR
int32 UMaterialExpressionWeather::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	return 0;
}

void UMaterialExpressionWeather::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("Weather"));
}
#endif