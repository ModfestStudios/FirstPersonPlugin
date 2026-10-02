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

/*********/
/*stamina*/
/*********/
protected:

UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina")
	float Stamina = 100.0f;
UPROPERTY(BLueprintReadOnly, EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.0f;
UPROPERTY(BLueprintReadOnly, EditAnywhere, Category = "Stamina")
	float ReserveStamina = 100.0f;
	/*the rate at which we gain stamina back (per second)*/
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina")
	float StaminaRecoveryRate = 7.65f;
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina")
	float ReserveStaminaRecoveryRate = .065f;


UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina|Consumption")
	float JumpingStaminaDrain = 12.0f;
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina|Consumption")
	float SoftLandStaminaDrain = 6.0f;
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina|Consumption")
	float DamagingLandStaminaDrain = 23.0f;
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina|Consumption")
	float SprintingStaminaBaseDrainRate = 6.45f;
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stamina|Consumption")
	float SprintingEffectiveStaminaDrainRate = 3.45f; 

	





/*************/
/*temperature*/
/*************/
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vitals|Temperature")
		float Temperature = 37.0f;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vitals|Temperature")
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

protected:
	UPROPERTY(Transient)
		class AFirstPersonCharacter* OwningCharacter;
	

	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================



public:	
	// Sets default values for this component's properties
	UVitalsComponent();
	virtual void InitializeComponent() override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//=========================
	//=========STAMINA=========
	//=========================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual bool HasStamina() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetCurrentStamina() const;
	/* the max stamina a character can have */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetMaxStamina() const;
	/*the max-stamina htis character has (effected by conditions)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetReserveStamina() const;



protected:
	UFUNCTION()
		virtual void CalculateStamina(float DeltaTime);
public:
		/*one-time event that'll consume stamina instantly*/
	UFUNCTION(BlueprintCallable, Category = "Vitals|Stamina")
		virtual void ConsumeStamina(float StaminaConsumption);
	UFUNCTION(BlueprintCallable, Category = "Vitals|Stamina")
		virtual void RecoverStamina(float StaminaRecovery);
	UFUNCTION(BlueprintCallable, Category = "Vitals|Stamina")
		virtual void RecoverReserveStamina(float StaminaRecovery);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetStaminaCostForJump() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetStaminaCostForSprinting() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetStaminaCostForSoftLanding() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Stamina")
		virtual float GetStaminaCostForDamagingLanding() const;

	//===========================
	//========TEMPERATURE========
	//===========================
protected:
	UFUNCTION()
		virtual void CalculateTemperature(float DeltaTime);
	UFUNCTION()
		virtual void ApplyDamageFromTemperature(float DeltaTime);


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Temperature")
		float GetPlayerTemperature();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Temperature")
		ETemperatureState GetTemperatureState();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Temperature")
		float GetAmbientTemperature();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Temperature")
		float GetHeatChange();
	



	UFUNCTION(BlueprintCallable, Category = "Vitals|Temperature")
		void AddTemperatureInfluence(class UTemperatureComponent* Source);
	UFUNCTION(BlueprintCallable, Category = "Vitals|Temperature")
		void RemoveTemperatureInfluence(class UTemperatureComponent* Source);

	//=============================
	//==========UTILITIES==========
	//=============================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vitals|Utilities")
		virtual class AFirstPersonCharacter* GetOwningCharacter() const;
protected:
		
	UFUNCTION()
		class UWeatherSubsystem* GetWeatherSubsystem();

		
};
