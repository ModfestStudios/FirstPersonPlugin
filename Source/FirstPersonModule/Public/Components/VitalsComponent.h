// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VitalsComponent.generated.h"




UENUM(BlueprintType)
enum class ETemperatureState : uint8
{
	Hyperthermic,
	Overheating,
	Warm,
	Regulated,
	Cold,
	Freezing,
	Hypothermic
	
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UVitalsComponent : public UActorComponent
{
	GENERATED_BODY()
public:


/*************/
/*temperature*/
/*************/
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Temperature")
		float Temperature = 37.0f;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature")
		float RegulatedTemperatureThreshold = 37.0f;

	/*hyper-thermia (overheating)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hyperthermia")
		float WarmThreshold = 37.5f;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hyperthermia")
		float HotThreshold = 38.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hyperthermia")
		float HyperthermiaThreshold = 39.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hyperthermia")
		float HyperthermiaDamageMin = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hyperthermia")
		float HyperthermiaDamageMax = 0.22f;


	/*hypo-thermia (freezing)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hypothermia")
		float ColdThreshold = 36.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hypothermia")
		float FreezingThreshold = 35.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hypothermia")
		float HypothermiaThreshold = 32.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hypothermia")
		float HypothermiaDamageMin = 0.14f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Temperature|Hypothermia")
		float HypothermiaDamageMax = 0.28f;

protected:

	/*any source that may affect our temperature*/
	UPROPERTY()
		TArray<class UTemperatureComponent*> TemperatureInfluences;

	/*rate of the characterr heating us up*/
	UPROPERTY()
		float HeatGain;	
	/*rate of the character cooling us down*/
	UPROPERTY()
		float HeatLoss;

	/*amount of exposure to hyperthermia conditions*/
	UPROPERTY()
		float HeatExposure;
	/*amuount of exposure to hypothermia conditions*/
	UPROPERTY()
		float ColdExposure;
	

	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================



public:	
	// Sets default values for this component's properties
	UVitalsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UFUNCTION()
		virtual void CalculateTemperature(float DeltaTime);
	UFUNCTION()
		virtual void ApplyDamageFromTemperature(float DeltaTime);



	//===========================
	//========TEMPERATURE========
	//===========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Temperature")
		float GetPlayerTemperature();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Temperature")
		ETemperatureState GetTemperatureState();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Temperature")
		float GetAmbientTemperature();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Temperature")
		float GetHeatChange();
	



	UFUNCTION(BlueprintCallable, Category = "Temperature")
		void AddTemperatureInfluence(class UTemperatureComponent* Source);
	UFUNCTION(BlueprintCallable, Category = "Temperature")
		void RemoveTemperatureInfluence(class UTemperatureComponent* Source);

	//=============================
	//==========UTILITIES==========
	//=============================
	UFUNCTION()
		class UWeatherSubsystem* GetWeatherSubsystem();

		
};
