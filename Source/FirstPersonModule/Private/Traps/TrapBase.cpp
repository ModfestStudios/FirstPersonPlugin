// Fill out your copyright notice in the Description page of Project Settings.


#include "Traps/TrapBase.h"

#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"

/*engine*/
#include "Engine/World.h"

/*utilities*/
#include "TimerManager.h"

/*network*/
#include "Net/UnrealNetwork.h"


void ATrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrap, bTrapActivated);
	DOREPLIFETIME(ATrap, bRetracted);
}

// Sets default values
ATrap::ATrap()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoActivate)
		ActivateTrap();	
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*trap activation timer*/
	if (bAutoReset && bTrapActivated)
	{
		if (HasElapsedTimeExceededDesiredDuration())
			ResetTrap();
	}
}

void ATrap::ActivateTrap()
{
	InternalActivationTime = ActivationTime + FMath::RandRange(ActivationTimeVariance * -1, ActivationTimeVariance);

	bTrapActivated = true;
	LastActiveTimestamp = GetWorld()->TimeSeconds;
}

void ATrap::ResetTrap()
{
	bTrapActivated = false;
}

bool ATrap::IsTrapActivated()
{
	return bTrapActivated;
}

void ATrap::OnRep_TrapActivated()
{
	if (bTrapActivated)
		ActivateTrap();
	else
		ResetTrap();
}

bool ATrap::IsTrapRetracted()
{
	return bRetracted;
}

void ATrap::RetractTrap()
{
	if (bRetracted == false)
		bRetracted = true;
}

void ATrap::ProtractTrap()
{
	if (bRetracted == true)
		bRetracted = false;
}

void ATrap::DamageActor(AActor* Target)
{
	/*we don't want clients applying damage directly*/
	if (!GetWorld() || GetWorld()->GetNetMode() == NM_Client)
		return;

	float CurrentDamage = bTrapActivated ? DamageAmount : bDamageWhenInactive ? InactiveDamageAmount : 0.0f;

	/*safety check*/
	if (!Target || CurrentDamage <= 0.0f)
		return;

	UGameplayStatics::ApplyDamage(Target, CurrentDamage, nullptr, this, DamageType);
}

void ATrap::AddActorToDamage(AActor* Target)
{
	ActorsToDamage.AddUnique(Target);
	DamageActor(Target); //initial damage

	/*enable repeat damage if on*/
	if (bUseDamageRate && !GetWorldTimerManager().IsTimerActive(DamageRateHandler) && ActorsToDamage.Num() > 0)
		GetWorldTimerManager().SetTimer(DamageRateHandler, this, &ATrap::DamageAllActors, DamageRate, true);
}

void ATrap::RemoveActorToDamage(AActor* Target)
{
	ActorsToDamage.Remove(Target);

	if (ActorsToDamage.Num() < 1 && GetWorldTimerManager().IsTimerActive(DamageRateHandler))
		GetWorldTimerManager().ClearTimer(DamageRateHandler);
}

void ATrap::DamageAllActors()
{
	for (AActor* Actor : ActorsToDamage)
	{
		DamageActor(Actor);
	}
}

void ATrap::PushCharacterBack(ACharacter* Character,const float BackwardStrength,const float UpwardStrength)
{
	if (!Character)
		return;

	FVector TrapLocation = GetActorLocation();
	FVector CharacterLocation = Character->GetActorLocation();
	FVector TossDirection = (CharacterLocation - TrapLocation).GetSafeNormal2D();

	TossDirection.Z = 0.0f;
	
	FVector LaunchVelocity = TossDirection * BackwardStrength + (FVector::UpVector * UpwardStrength);

	Character->LaunchCharacter(LaunchVelocity, true, true);
}

float ATrap::GetActiveElapsedTime()
{
	/*return 0.0f if we've already been reset*/
	if (!bTrapActivated)
		return 0.0f;

	return GetWorld()->TimeSeconds - LastActiveTimestamp;
}

bool ATrap::HasElapsedTimeExceededDesiredDuration()
{	
	float ElapsedTime = GetActiveElapsedTime();

	return ElapsedTime > InternalActivationTime;	
}

