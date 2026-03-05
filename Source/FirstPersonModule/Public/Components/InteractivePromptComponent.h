// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractivePromptComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "Interactives", meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UInteractivePromptComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:


	//=================================================================================================================
	//====================================================FUNCTIONS====================================================
	//=================================================================================================================

	UInteractivePromptComponent();

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;	
	void PostEditComponentMove(bool bFinished) override;
#endif
	void InitializeComponent() override;
	void OnRegister() override;	



	//void PostEditChange() override;
	void BeginPlay() override;

	void SetWorldSpecificComponentVisiblity();

public:
	UFUNCTION()
		virtual void SetPromptText(FText NewText);

	
};
