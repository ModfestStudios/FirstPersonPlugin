// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/VitalsComponent.h"

/*components*/
#include "Components/TemperatureComponent.h"

/*engine*/
#include "Engine/World.h"
#include "GameModes/FirstPersonWorldSettings.h"
#include "Subsystems/WeatherSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

// Sets default values for this component's properties
UVitalsComponent::UVitalsComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;
}


// Called when the game starts
void UVitalsComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UVitalsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CalculateTemperature(DeltaTime);
	ApplyDamageFromTemperature(DeltaTime);
}

void UVitalsComponent::CalculateTemperature(float DeltaTime)
{
	//grab the overall exterior temperature of the world first
	float Gain = 0.0f;
	float Loss = 0.0f;
	float AmbientRate = GetWeatherSubsystem()->GetAmbientTemperatureEffectRate();

	AmbientRate < 0.0f ? Loss += AmbientRate : Gain += AmbientRate;

	// add/subtract local influences


	//add/subtract based on medical conditions

	//update total heat gain or heat loss
	HeatGain = Gain;
	HeatLoss = Loss;

	Temperature += GetHeatChange() * DeltaTime;
}

void UVitalsComponent::ApplyDamageFromTemperature(float DeltaTime)
{

}

float UVitalsComponent::GetPlayerTemperature()
{
	return Temperature;
}

ETemperatureState UVitalsComponent::GetTemperatureState()
{
	/*properly regulated range*/
	if (Temperature < WarmThreshold && Temperature > ColdThreshold)
		return ETemperatureState::Regulated;

	/*hypo-thermia (being cold)*/
	if (Temperature <= ColdThreshold)
	{
		if (Temperature <= HypothermiaThreshold)
			return ETemperatureState::Hypothermic;
		if (Temperature <= FreezingThreshold)
			return ETemperatureState::Freezing;
		else
			return ETemperatureState::Cold;
	}

	/*hyper-thermia (being hot)*/
	if (Temperature >= WarmThreshold)
	{
		if (Temperature >= HyperthermiaThreshold)
			return ETemperatureState::Hyperthermic;
		if (Temperature >= HotThreshold)
			return ETemperatureState::Overheating;
		else
			return ETemperatureState::Warm;
	}
	
	return ETemperatureState::Regulated;
}

float UVitalsComponent::GetAmbientTemperature()
{
	if (TemperatureInfluences.Num() > 0)
		return TemperatureInfluences[0]->AmbientTemperature;
	
	
	if (UWorld* World = GetWorld())
	{
		if (UWeatherSubsystem* WSS = GetWorld()->GetSubsystem<UWeatherSubsystem>())
			return WSS->GetTempatureAsCelsius();

			/*if (AFirstPersonWorldSettings* WorldSettings = Cast<AFirstPersonWorldSettings>(World->GetWorldSettings()))
			return WorldSettings->GetAmbientTemperature();*/
	}

	/*otherwise return a calm 85.00 degrees because everything is broken*/	
	return 37.0f;
}

float UVitalsComponent::GetHeatChange()
{
	return FMath::Abs(HeatGain) - FMath::Abs(HeatLoss);
}

void UVitalsComponent::AddTemperatureInfluence(UTemperatureComponent* Source)
{
	TemperatureInfluences.AddUnique(Source);
}

void UVitalsComponent::RemoveTemperatureInfluence(UTemperatureComponent* Source)
{
	TemperatureInfluences.Remove(Source);
}

UWeatherSubsystem* UVitalsComponent::GetWeatherSubsystem()
{
	if (GetWorld())
	{
		return GetWorld()->GetSubsystem<UWeatherSubsystem>();
	}

	return nullptr;
}

