// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OperatingSystemApplicationWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UOperatingSystemApplicationWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application")
		bool bCreateApplicationOnInitialize = true;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application")
		TSubclassOf<class AOperatingSystemApplication> ApplicationClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application", meta = (EditCondition = bCreateApplicationOnInitialize))
		TSubclassOf<class AOperatingSystem> OperatingSystemClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Application", meta = (EditCondition = bCreateApplicationOnInitialize))
		TArray<FString> ApplicationLaunchParameters;


	UPROPERTY(BlueprintReadOnly, Category = "Application")
		class AOperatingSystemApplication* Application;

	virtual void NativeOnInitialized() override;
	
};
