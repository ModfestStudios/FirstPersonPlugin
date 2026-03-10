// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "WeatherVolume.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AWeatherVolume : public ATriggerVolume
{
	GENERATED_BODY()
public:



	UPROPERTY(VisibleAnywhere, Category = "Temperature")
		class UTemperatureComponent* TemperatureComponent;


	AWeatherVolume(const FObjectInitializer& ObjectInitializer);


	UFUNCTION()
	virtual void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	virtual void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);




	
};
