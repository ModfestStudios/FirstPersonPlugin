// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/VitalsComponent.h"

#include "Characters/FirstPersonCharacter.h"

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
	PrimaryComponentTick.TickInterval = 0.025f;
}

void UVitalsComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OwningCharacter = Cast<AFirstPersonCharacter>(GetOwner());
}


// Called when the game starts
void UVitalsComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!OwningCharacter)
		OwningCharacter = Cast<AFirstPersonCharacter>(GetOwner());
}


// Called every frame
void UVitalsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*calculate first*/
	CalculateTemperature(DeltaTime);
	CalculateStamina(DeltaTime);

	/*apply damages*/
	ApplyDamageFromTemperature(DeltaTime);
}

bool UVitalsComponent::HasStamina() const
{
	return Stamina > 0.0f;
}

float UVitalsComponent::GetCurrentStamina() const
{
	return Stamina;
}

float UVitalsComponent::GetMaxStamina() const
{
	return MaxStamina;
}

float UVitalsComponent::GetReserveStamina() const
{
	return ReserveStamina;
}

void UVitalsComponent::CalculateStamina(float DeltaTime)
{	
	float StaminaConsumption = 0.0f;
	float StaminaRecovery = StaminaRecoveryRate * DeltaTime;
	float EffectiveRecovery = ReserveStaminaRecoveryRate * DeltaTime;	
	
	
	StaminaConsumption += GetStaminaCostForSprinting() * DeltaTime;

	/*apply affect if we've consumed stmaina*/
	if(StaminaConsumption > 0.0f)
		ConsumeStamina(StaminaConsumption);
	else
	{
		/*prevent stamina regeneration while falling as it feels weird to jump and immedietely recover while still in the air*/
		if(GetOwningCharacter() && GetOwningCharacter()->IsFalling())
			return;

		if(FMath::IsNearlyEqual(GetReserveStamina(), GetMaxStamina()))
			RecoverStamina(StaminaRecovery);
		else
		{
			RecoverReserveStamina(EffectiveRecovery);
			RecoverStamina(StaminaRecovery * 0.65f); //penalty applied			
		}
	}		
}

void UVitalsComponent::ConsumeStamina(float StaminaConsumption)
{
	if (StaminaConsumption <= 0.0f)
		return;

	const float StaminaConsumed = FMath::Min(Stamina, StaminaConsumption);

	Stamina -= StaminaConsumed;
	StaminaConsumption -= StaminaConsumed;

	/*we've exceeded our immediate stamina, begin over-exerting*/
	if (StaminaConsumption > 0.0f)
	{
		ReserveStamina = FMath::Max(0.0f,ReserveStamina - StaminaConsumption);
	}
}

void UVitalsComponent::RecoverStamina(float StaminaRecovery)
{	
	if(StaminaRecovery <= 0.0f)
		return;

	Stamina = FMath::Min(GetReserveStamina(), Stamina + StaminaRecovery);
	
}

void UVitalsComponent::RecoverReserveStamina(float StaminaRecovery)
{
	if(StaminaRecovery <= 0.0f)
		return;

	ReserveStamina = FMath::Min(GetMaxStamina(), ReserveStamina + StaminaRecovery);
}

float UVitalsComponent::GetStaminaCostForSprinting() const
{
	if (GetOwningCharacter() && GetOwningCharacter()->IsSprinting())
	{
		if(GetCurrentStamina() > 0.0f)
			return SprintingStaminaBaseDrainRate;
		else
			return SprintingEffectiveStaminaDrainRate;
	}
		
	else
		return 0.0f;
}

float UVitalsComponent::GetStaminaCostForSoftLanding() const
{
	return SoftLandStaminaDrain;
}

float UVitalsComponent::GetStaminaCostForDamagingLanding() const
{
	return DamagingLandStaminaDrain;
}

float UVitalsComponent::GetStaminaCostForJump() const
{
	return JumpingStaminaDrain;
}


//===========================
//========TEMPERATURE========
//===========================



void UVitalsComponent::CalculateTemperature(float DeltaTime)
{
	//grab the overall exterior temperature of the world first
	float Gain = 0.0f;
	float Loss = 0.0f;
	float AmbientRate = GetWeatherSubsystem() ? GetWeatherSubsystem()->GetAmbientTemperatureEffectRate() : 0.0f;

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

AFirstPersonCharacter* UVitalsComponent::GetOwningCharacter() const
{
	return OwningCharacter;
}

UWeatherSubsystem* UVitalsComponent::GetWeatherSubsystem()
{
	if (GetWorld())
	{
		return GetWorld()->GetSubsystem<UWeatherSubsystem>();
	}

	return nullptr;
}

