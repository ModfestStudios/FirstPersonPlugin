// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SensesComponent.h"
#include "AI/SensesEvaluationProfile.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*curves*/
#include "Curves/CurveFloat.h"
#include "Curves/RichCurve.h"

/*engine*/
#include "Engine/World.h"

/*framework*/
#include "GameFramework/Actor.h"

/*players*/
#include "AI/FirstPersonAIController.h"

/*subsystems*/
#include "Subsystems/AIBehaviorSubsystem.h"
#include "Subsystems/CharacterSubsystem.h"

// Sets default values for this component's properties
USensesComponent::USensesComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}


// Called when the game starts
void USensesComponent::BeginPlay()
{
	Super::BeginPlay();


	if (GetWorld())
		GetWorld()->GetSubsystem<UAIBehaviorSubsystem>()->RegisterSensesComponent(this);
}


// Called every frame
void USensesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//EvaluateSensedActors(DeltaTime);	
}

void USensesComponent::EvaluateSenses(USensesEvaluationProfile* SensesProfile, float DeltaTime)
{
	if (!SensesProfile)
		return;


	if (const TSet<AActor*>* ActorsToEvaluate = GetActorsToEvaluate())
	{
	/*loop through actors*/
		for (AActor* Actor : *ActorsToEvaluate)
		{
			/*validation check*/
			if (!IsValid(Actor) || Actor == GetOwner())
				continue;

			/*if they've not been sensed we need to do a quick check to see if they're within the realm of possible*/
			if (!SensedActors.Contains(Actor))
			{
				bool bShouldAdd = false;
				/*quick vision-potential check*/
				if (SensesProfile->bHasVision && IsWithinVisualRange(Actor,SensesProfile->MaxVisionDistance))
					bShouldAdd = true;



				if (bShouldAdd)
				{
					FSensesEvaluation ActorEvaluation = FSensesEvaluation();
					ActorEvaluation.AwarenessState = EActorAwarenessState::Unaware; //make sure they start off being unaware of
					ActorEvaluation.TimeInSightToDetect = FMath::FRandRange(SensesProfile->VisualDetectionDelayRange.X, SensesProfile->VisualDetectionDelayRange.Y); //generate a small variation to how long it takes to spot them

					/*add*/
					SensedActors.Add(Actor, ActorEvaluation);
				}
			}
		}
	}


	if (SensesProfile->bHasVision)
		EvaluateVision(SensesProfile, DeltaTime);

	//if(!SensesProfile)
	//	return;


	///*init*/
	//if (const TSet<AActor*>* ActorsToEvaluate = GetActorsToEvaluate())
	//{

	//}



	///*start by seeing who hasn't been sensed yet - and evaluate if they're within any of our sensory ranges*/
	//for (int32 i = 0; i < ActorsToEvaluate.Num(); i++)
	//{
	//	AActor* Actor = ActorsToEvaluate[i];

	//	if (IsValid(Actor))
	//	{
	//		/*if they've not yet been sensed - evaulate if they should be*/
	//		if (!SensedActors.Contains(Actor))
	//		{				
	//			if ((bHasVision && IsWithinVisualRange(Actor)))
	//			{
	//				FSensesEvaluation ActorEvaluation = FSensesEvaluation();
	//				ActorEvaluation.AwarenessState = EActorAwarenessState::Unaware; //Makes sure we're unaware of them at first
	//				ActorEvaluation.TimeInSightToDetect = FMath::FRandRange(VisualDetectionDelayRange.X,VisualDetectionDelayRange.Y); //Gives a random variance to how quickly we spot this actor visually
	//				
	//				/*add to evaluation list*/
	//				SensedActors.Add(Actor, ActorEvaluation);					
	//			}
	//		}
	//	}
	//	else
	//		continue;

	//}

	///*now that we've marked who falls within our senses - let's go ahead and evaluate our latest updates about them*/
	///*update vision*/
	//if (bHasVision)
	//	EvaluateVision(DeltaTime);
}

void USensesComponent::EvaluateVision(USensesEvaluationProfile* SensesProfile, float DeltaTime)
{
	/*loop through actors within our senses*/
	for (TPair<TWeakObjectPtr<AActor>, FSensesEvaluation>& Pair : SensedActors)
	{
		/*init evaluation*/
		AActor* Actor = Pair.Key.Get();
		FSensesEvaluation& Evaluation = Pair.Value;

		/*skip if invalid*/
		if(!Actor)
			continue;
		
		
		if (IsWithinVisualRange(Actor, SensesProfile->MaxVisionDistance))
		{
			Evaluation.TimeInSight += DeltaTime;

			if (Evaluation.AwarenessState >= EActorAwarenessState::Detected)
			{
				if (Evaluation.bCurrentlySeen)
				{
					Evaluation.LastSeenLocation = Actor->GetActorLocation();
				}
			}

			if (Evaluation.bCurrentlySeen == false && HasActorExceededVisualDetectionTime(Actor))
				Evaluation.bCurrentlySeen = true;

			if (Evaluation.AwarenessState < EActorAwarenessState::Detected && Evaluation.bCurrentlySeen == true)
				Evaluation.AwarenessState = EActorAwarenessState::Detected;

			/*small buffer that prevents TimeSinceSightLost from resetting instantly if AI loses sight of Actor for a brief moment*/
			if(Evaluation.TimeInSight > SensesProfile->TimeBeforeSightReset)
				Evaluation.TimeSinceSightLost = 0.0f;			
		}			
		else
		{
			Evaluation.TimeSinceSightLost += DeltaTime;
			
			Evaluation.bCurrentlySeen = false;

			if (Evaluation.AwarenessState >= EActorAwarenessState::Detected)
			{
				Evaluation.AwarenessState = EActorAwarenessState::Lost;
			}

			/*small buffer that prevents TimeInSight from resetting instantly if AI loses sight of Actor for a brief moment*/
			if (Evaluation.TimeSinceSightLost > SensesProfile->TimeBeforeSightReset)
				Evaluation.TimeInSight = 0.0f;
		}		
		
		Evaluation.SightStrength = GetVisualStrengthAgainstActor(Actor,SensesProfile->VisualFalloff,SensesProfile->MaxVisionDistance);
		Evaluation.Distance = GetDistanceToActor(Actor);
		
		}
		
}

//TArray<FActorEvaluation> USensesComponent::GetAllKnownActors()
//{
//	return SensedActors;
//}

int32 USensesComponent::GetNumOfActorsSensing()
{
	return SensedActors.Num();
}

FSensesEvaluation USensesComponent::GetActorEvaluation(AActor* Actor) const
{
	if(const FSensesEvaluation* Evalation = SensedActors.Find(Actor))
		return *SensedActors.Find(Actor);
	else
		return FSensesEvaluation();
}

float USensesComponent::GetDistanceToActor(AActor* Actor)
{
	if (!Actor || !GetOwner())
		return 0.0f;

	return GetOwner()->GetDistanceTo(Actor);
}

bool USensesComponent::IsWithinVisualRange(AActor* Actor, float MaxVisionDistance)
{
	if (!Actor)
		return false;

	return GetDistanceToActor(Actor) <= MaxVisionDistance;
}

bool USensesComponent::HasActorExceededVisualDetectionTime(AActor* Actor)
{
	if (const auto& Evaluation = SensedActors.Find(Actor))
	{
		return Evaluation->TimeInSight >= Evaluation->TimeInSightToDetect;
	}

	return false;
}

float USensesComponent::GetVisualStrengthAgainstActor(AActor* Actor, FRuntimeFloatCurve VisualFallofCurve, float MaxVisionDistance)
{
	if(!IsWithinVisualRange(Actor, MaxVisionDistance))
		return 0.0f;

	const FRichCurve* VisualCurve = VisualFallofCurve.GetRichCurveConst();

	if (VisualCurve && !VisualCurve->IsEmpty())
	{
		float NormalizedDistance = FMath::Clamp(GetDistanceToActor(Actor) / MaxVisionDistance, 0.0f, 1.0f);
		float VisualStrength = VisualCurve->Eval(NormalizedDistance);

		/*truncate to two decimal places*/
		VisualStrength *= 100;
		VisualStrength = FMath::TruncToFloat(VisualStrength);
		VisualStrength /= 100;
		
		return VisualStrength;
	}
	else
		return 1.0f;	
}

TArray<AActor*> USensesComponent::GetDetectedActors() const
{
	if(SensedActors.Num() <= 0)
		return TArray<AActor*>();

	TArray<AActor*> DetectedActors;
	for (const auto& Pair : SensedActors)
	{
		const AActor* Actor = Pair.Key.Get();
		const FSensesEvaluation& Evaluation = Pair.Value;

		/*skip invalid entries*/
		if(!Actor)
			continue;

		if(Evaluation.AwarenessState == EActorAwarenessState::Detected)
			DetectedActors.Add(const_cast<AActor*>(Actor));
	}

	return DetectedActors;
}

int32 USensesComponent::GetNumberOfDetectedActors() const
{
	if (SensedActors.Num() <= 0)
		return 0;

	int32 NumOfDetectedActors = 0;
	for (const auto& Pair : SensedActors)
	{
		const AActor* Actor = Pair.Key.Get();
		const FSensesEvaluation& Evaluation = Pair.Value;

		/*skip invalid entries*/
		if (!Actor)
			continue;

		if (Evaluation.AwarenessState == EActorAwarenessState::Detected)
			NumOfDetectedActors++;
	}

	return NumOfDetectedActors;
}



const TSet<AActor*>* USensesComponent::GetActorsToEvaluate() const
{
	if (UAIBehaviorSubsystem* BSS = GetWorld()->GetSubsystem<UAIBehaviorSubsystem>())
		return &BSS->GetRegisteredActors();
	else
		return nullptr;
}

//TArray<AActor*> USensesComponent::GetActorsToEvaluate()
//{
//	TArray<AActor*> Actors;
//
//	if (UCharacterSubsystem* CSS = GetWorld()->GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
//	{
//		for (AActor* PlayerPawn : CSS->GetAllPlayerPawns())
//		{
//			Actors.AddUnique(PlayerPawn);
//		}
//	}
//
//	return Actors;
//}

