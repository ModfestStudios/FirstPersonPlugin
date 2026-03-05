// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InteractiveComponent.h"
#include "CounterComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCounterEvent, AActor*, Owner, UCounterComponent*, CounterComponent);

UENUM(BlueprintType)
enum class ECounterType : uint8
{
	Countdown,
	Stopwatch
};


/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UCounterComponent : public UInteractiveComponent
{
	GENERATED_BODY()
public:


	/**/
	UPROPERTY(EditAnywhere, Category = "Counter")
		ECounterType Type = ECounterType::Countdown;

	/*how long the countdown is*/
	UPROPERTY(EditAnywhere, Category = "Counter|Countdowns")
		float CountdownDuration = 5.0f;
	/*last mark of when a countdown started*/
	UPROPERTY(BlueprintReadOnly, Category = "Counter|Counntdowns")
		float StartTimeStamp;
	UPROPERTY(BlueprintReadOnly, Category = "Counter|Counntdowns")
		float StopTimeStamp;

	FTimerHandle CountdownHandler;



	/*events*/
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FCounterEvent OnCountdownStarted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FCounterEvent OnCountdownFinished;


	//===========================================================================================================================================
	//=================================================================FUNCTIONS=================================================================
	//===========================================================================================================================================
	UCounterComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



public:
	UFUNCTION(BlueprintCallable, Category = "Counter")
		virtual void StartCounter();
	UFUNCTION(BlueprintCallable, Category = "Counter")
		virtual void StopCounter();
private:
	/*replicated version that passes the time the server started/stopped at - allowing clients to all sync the stop exactly*/
	UFUNCTION(Reliable, NetMulticast)
		void Multicast_StopCounter(float StartStamp, float StopStamp);

	//===========================
	//=========COUNTDOWN=========
	//===========================
protected:
	UFUNCTION()
		virtual void StartCountdown();
	UFUNCTION()
		virtual void MarkStartTime();
	UFUNCTION()
		virtual void NativeOnCountdownFinished();
public:
	UFUNCTION(BlueprintCallable, Category = "Counter|Countdown")
		virtual void SetCountdownDuration(float Duration);
	UFUNCTION(BlueprintCallable, Category = "Counter|Countdown")
		bool IsCountdownActive();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Counter|Countdown")
		float GetTimeRemaining();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Counter|Countdown")
		FText GetTimeRemainingAsText();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Counter|Countdown")
		float GetTimeRunning();



protected:
	//===============================
	//==========UTILITITIES==========
	//===============================	
	class FTimerManager& GetTimerManager() const;


	

	
};
