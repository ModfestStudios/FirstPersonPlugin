// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CounterComponent.h"
#include "TimerManager.h"
#include "../Logging.h"

/*engine*/
#include "Engine/World.h"

UCounterComponent::UCounterComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCounterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCounterComponent::StartCounter()
{
	if (Type == ECounterType::Countdown)
		StartCountdown();
	
	
}

void UCounterComponent::StopCounter()
{
	StopTimeStamp = GetWorld()->TimeSeconds;
	Multicast_StopCounter(StartTimeStamp, StopTimeStamp);	
}

void UCounterComponent::Multicast_StopCounter_Implementation(float StartStamp, float StopStamp)
{
	StartTimeStamp = StartStamp;
	StopTimeStamp = StopStamp;	
	GetTimerManager().ClearTimer(CountdownHandler);
}

void UCounterComponent::StartCountdown()
{
	if (IsCountdownActive())
	{
		UE_LOG(LogFirstPersonModule, Log, TEXT("%s::StartCount() - Countdown already active - use ResetCountdown() or stop manually first"),*GetName());
		return;
	}

	/*mark our timestamp*/
	MarkStartTime();
	/*set and activate a timer*/
	GetTimerManager().SetTimer(CountdownHandler, this, &UCounterComponent::NativeOnCountdownFinished,CountdownDuration,false);

	/*notify anyone listening*/
	if (OnCountdownStarted.IsBound())
		OnCountdownStarted.Broadcast(GetOwner(), this);
}

void UCounterComponent::MarkStartTime()
{
	if (!GetWorld())
		return;

	StartTimeStamp = GetWorld()->TimeSeconds;
}

void UCounterComponent::NativeOnCountdownFinished()
{		


	if (OnCountdownFinished.IsBound())
		OnCountdownFinished.Broadcast(GetOwner(), this);
}

void UCounterComponent::SetCountdownDuration(float Duration)
{
	CountdownDuration = Duration;
}

bool UCounterComponent::IsCountdownActive()
{
	return GetTimerManager().IsTimerActive(CountdownHandler);
}

float UCounterComponent::GetTimeRemaining()
{
	if (!GetWorld())
		return 0.0f;
	
	if (GetTimeRunning() >= CountdownDuration)
		return 0.0f;

	/*calculate time remaining*/
	float TimeRemaining = CountdownDuration - GetTimeRunning();

	/*round to 2-decimal place (by shifting the decimal, truncating, then shifting it back)*/
	TimeRemaining = FMath::RoundToFloat(TimeRemaining * 100.0f);	
	TimeRemaining = TimeRemaining / 100.0f;

	return TimeRemaining;
}

FText UCounterComponent::GetTimeRemainingAsText()
{
	return FText::AsNumber(GetTimeRemaining(), &FNumberFormattingOptions().SetMinimumFractionalDigits(2).SetMaximumFractionalDigits(2));
}

float UCounterComponent::GetTimeRunning()
{
	if (!GetWorld())
		return 0.0f;

	if (IsCountdownActive())
		StopTimeStamp = GetWorld()->TimeSeconds;

	/*calculate how long we've been runing based on timestmap*/
	float TimeRunning = StopTimeStamp - StartTimeStamp;
	/*round to 2-decimal place (by shifting the decimal, truncating, then shifting it back)*/
	TimeRunning = FMath::RoundToFloat(TimeRunning * 100.0f);
	TimeRunning = TimeRunning / 100.0f;
	
	return TimeRunning;
}

FTimerManager& UCounterComponent::GetTimerManager() const
{
	return GetWorld()->GetTimerManager();
}
