// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UMapSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:


	/*all available maps*/
	UPROPERTY()
		TArray<class UMapAsset*> Maps;


	//=========================================================================================================================================
	//================================================================FUNCTIONS================================================================
	//=========================================================================================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;



	//======================
	//=========MAPS=========
	//======================
	UFUNCTION()
		void InitializeDefaultMaps();
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Maps", meta = (DeterminesOutputType="MapClass", DynamicOutputParam="OutMaps"))
		void GetMapsByClass(TSubclassOf<UMapAsset> MapClass, TArray<UMapAsset*>& OutMaps) const;

	
};
