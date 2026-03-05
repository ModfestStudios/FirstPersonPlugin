// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterNameAsset.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UCharacterNameAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
		TArray<FString> MaleFirstNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
		TArray<FString> FemaleFirstNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
		TArray<FString> LastNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
		TArray<FString> Callsigns;
	
};
