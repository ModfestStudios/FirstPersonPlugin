// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CinemaSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UCinemaSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	/*the current scene capture component being rendered*/
	UPROPERTY()
		class AActor* ActiveViewTarget;

	/*render target to use when an actor doesn't have a render target*/
	UPROPERTY()
		TObjectPtr<class UTextureRenderTarget2D> FallbackRenderTarget;
	/*the material the movie theater screen utilizes to render targets*/
	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicMaterial;
	/*Name of Texture Param inside the material we adjust for active render target*/
	UPROPERTY()
		FName RenderTargetMaterialName = FName("Render Target");





	//===============================================================================================================================
	//===========================================================FUNCTIONS===========================================================
	//===============================================================================================================================
	




	UFUNCTION(BlueprintCallable, Category = "Cinema Subsystem|View Targets")
		virtual void SetViewTarget(class AActor* NewViewTarget);


	/*creates and sets reference for Dynamic Material to be used by Subsystem from the passed in material type*/
	UFUNCTION(BlueprintCallable, Category = "Cinema Subsystem|Materials")
		class UMaterialInstanceDynamic* InitiateSceneCaptureMaterial(class UMaterialInterface* Material);
	/*return the active material being used for scene capture*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinema Subsystem|Materials")
		class UMaterialInstanceDynamic* GetSceneCaptureMaterial();
	UFUNCTION(BlueprintCallable, Category = "Cinema Subsystem|Materials")
		virtual void SetFallbackRenderTarget(class UTextureRenderTarget2D* RenderTarget);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cinema Subsystem|View Targets")
		class USceneCaptureComponent2D* GetActiveSceneCaptureComponent();
};
