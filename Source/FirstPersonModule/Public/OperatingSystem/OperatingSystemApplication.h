// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "OperatingSystemApplication.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, abstract)
class FIRSTPERSONMODULE_API AOperatingSystemApplication : public AInfo
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY()
		class AOperatingSystem* OperatingSystem;

	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================
	

public:
	UFUNCTION()
		virtual void OnApplicationLaunched(const TArray<FString>& Parameters, class AOperatingSystem* OS);
	UFUNCTION(BlueprintImplementableEvent, Category = "Operating System|Application", meta = (DisplayName = "On Application Launched"))
		void BP_OnApplicationLaunched(const TArray<FString>& Parameters);


	UFUNCTION()
		virtual void OnApplicationClosed();
	UFUNCTION(BlueprintImplementableEvent, Category = "Operating System|Application", meta = (DisplayName = "On Application Closed"))
		void BP_OnApplicationClosed();




	UFUNCTION(BlueprintCallable, Category = "Operating System|Application")
		class AOperatingSystem* GetOperatingSystem();




	
};
