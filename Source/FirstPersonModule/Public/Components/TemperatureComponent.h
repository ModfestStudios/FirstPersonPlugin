// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TemperatureComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UTemperatureComponent : public UActorComponent
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Temperature")
		float AmbientTemperature = 33.0f;



public:	
	// Sets default values for this component's properties
	UTemperatureComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
