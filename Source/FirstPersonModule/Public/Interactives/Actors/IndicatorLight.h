// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IndicatorLight.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API AIndicatorLight : public AActor
{
	GENERATED_BODY()
public:
		
	/*since the indicator is going to be either on or off - we can simply replicate this variable to everyone for easy control of its state*/
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_IndicatorToggled, EditAnywhere, Category = "Indicator")
		bool bIsTurnedOn = false;
	UPROPERTY(Replicated, EditAnywhere, Category = "Indicator")
		float Intensity = 1.0f;
	/*set the name of the material you wish to turn into a Dynamic Material Instance - during BeginPlay() the IndictatorLight will auto-create a dynamic material instance from the found material using the provided name*/
	UPROPERTY(EditAnywhere, Category = "Indicator")
		FLinearColor LightColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
		FName LightMaterialName = "Light";
	UPROPERTY(VisibleAnywhere, Category = "Indicator")
		FName ColorParameterName = "Color";
	UPROPERTY(VisibleAnywhere, Category = "Indicator")
		FName IntensityParameterName = "Intensity";	
	UPROPERTY()
		class UMaterialInstanceDynamic* MaterialInstance;



	/*mesh*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Rendering")
		class UStaticMeshComponent* Mesh;

	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================
public:	
	// Sets default values for this actor's properties
	AIndicatorLight(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Indicator")
		virtual void TurnOn();
	UFUNCTION(BlueprintCallable, Category = "Indicator")
		virtual void TurnOff();
	UFUNCTION(BlueprintCallable, Category = "Indicator")
		virtual void Toggle();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Indicator")
		virtual bool IsTurnedOn();
	UFUNCTION()
		virtual void OnRep_IndicatorToggled();

	UFUNCTION(BlueprintCallable, Category = "Indicator")
		virtual void SetLightColor(FLinearColor NewColor);
	UFUNCTION(BlueprintCallable, Category = "Indicator")
		virtual void SetIntensity(float NewIntensity);

	//====================
	//=======EVENTS=======
	//====================
protected:
	UFUNCTION()
		virtual void OnTurnedOn();
	UFUNCTION(BlueprintImplementableEvent, Category = "Indicator", meta = (DisplayName = "OnTurnedOn"))
		void BP_OnTurnedOn();
	UFUNCTION()
		virtual void OnTurnedOff();
	UFUNCTION(BlueprintImplementableEvent, Category = "Indicator", meta = (DisplayName = "OnTurnedOff"))
		void BP_OnTurnedOff();



	//========================
	//========UTILITIES=======
	//========================
protected:

	/*creates and sets the material instance as dynamic one so we can modify it during runtime*/
	UFUNCTION()
		void InitMaterialInstance();


};
