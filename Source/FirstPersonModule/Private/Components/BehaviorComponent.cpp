// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BehaviorComponent.h"

/*ai*/
#include "AI/ThreatEvaluationProfile.h"
#include "AI/FirstPersonAIController.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*components*/
#include "Components/SensesComponent.h"


// Sets default values for this component's properties
UBehaviorComponent::UBehaviorComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;	
}


// Called when the game starts
void UBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	InitBehaviorTree();	
}

void UBehaviorComponent::InitBehaviorTree()
{
	if (!BehaviorTree)
		return;

	if (APawn* P = Cast<APawn>(GetOwner()))
	{
		if (AFirstPersonAIController* AIC = Cast<AFirstPersonAIController>(P->GetController()))
			AIC->RunBehaviorTree(BehaviorTree);
	}
}


// Called every frame
void UBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


//===============================
//============TARGETS============
//===============================

void UBehaviorComponent::EvaluateTargets(UTargetEvaluationProfile* TargetProfile, float DeltaTime)
{
	if(!TargetProfile)
		return;
}


//===============================
//============THREATS============
//===============================

void UBehaviorComponent::EvaluateThreats(UThreatEvaluationProfile* ThreatProfile, float DeltaTime)
{
	if(!ThreatProfile)
		return;

	if (USensesComponent* SensesComponent = GetOwningPawnSensesComponent())
	{
		for (AActor* DetectedActor : SensesComponent->GetDetectedActors())
		{
			if (IsValid(DetectedActor))
			{
					FActorEvaluation& ActorEvaluation = EvaluatedActors.FindOrAdd(DetectedActor);
					FThreatEvaluation& ThreatEvaluation = ActorEvaluation.Threat;
					FSensesEvaluation SensesEvaluation = SensesComponent->GetActorEvaluation(DetectedActor);

					if (ThreatProfile->bEvaluateDistance)
					{
						float DistanceToActor = SensesEvaluation.Distance;
						ThreatEvaluation.Distance = GetCurveScore(ThreatProfile->DistanceScore, DistanceToActor);
					}
			}
			else
				continue;
		}
	}
}

float UBehaviorComponent::GetCurveScore(const FRuntimeFloatCurve& CurveProfile, const float ActorDistance)
{
	const FRichCurve* RichCurve = CurveProfile.GetRichCurveConst();


	if(!RichCurve || RichCurve->IsEmpty())
		return 0.0f;

	/*initialize*/
	float MinCurveTime = 0.0f;
	float MaxCurveTime = 0.0f;
	RichCurve->GetTimeRange(MinCurveTime, MaxCurveTime);

	/*clamp distance to min/max of the curve so we don't go out of bounds*/
	const float ClampedDistance = FMath::Clamp(ActorDistance,MinCurveTime,MaxCurveTime);

	/*return the results*/
	return RichCurve->Eval(ClampedDistance);
}

AActor* UBehaviorComponent::GetHighestThreat()
{
	AActor* HighestThreat = nullptr;
	float CurrentThreatLevel = 0.0f;
	for (TPair<TWeakObjectPtr<AActor>, FActorEvaluation>& Pair : EvaluatedActors)
	{
		AActor* Actor = Pair.Key.Get();
		FActorEvaluation& Evaluation = Pair.Value;
		float ThreatScore = Evaluation.Threat.GetThreatScore();

		if(!HighestThreat || ThreatScore > CurrentThreatLevel)
			HighestThreat = Actor;		
	}

	return HighestThreat;
}

AActor* UBehaviorComponent::GetLowestThreat()
{
	AActor* LowestThreat = nullptr;
	float CurrentThreatLevel = 0.0f;
	for (TPair<TWeakObjectPtr<AActor>, FActorEvaluation>& Pair : EvaluatedActors)
	{
		AActor* Actor = Pair.Key.Get();
		FActorEvaluation& Evaluation = Pair.Value;
		float ThreatScore = Evaluation.Threat.GetThreatScore();

		if (!LowestThreat || ThreatScore < CurrentThreatLevel)
			LowestThreat = Actor;
	}

	return LowestThreat;
}

AActor* UBehaviorComponent::GetClosestThreat()
{
	AActor* ClosestActor = nullptr;
	float CurrentClosestDistance = 0.0f;

	for (TPair<TWeakObjectPtr<AActor>, FActorEvaluation>& Pair : EvaluatedActors)
	{
		AActor* Actor = Pair.Key.Get();
		FActorEvaluation& Evaluation = Pair.Value;
		float Distance = Evaluation.Threat.Distance;

		if (!ClosestActor || Distance < CurrentClosestDistance)
		{
			ClosestActor = Actor;
			CurrentClosestDistance = Distance;
		}			
	}

	return ClosestActor;
}


//=================================
//============UTILITIES============
//=================================

USensesComponent* UBehaviorComponent::GetOwningPawnSensesComponent() const
{
	if(AFirstPersonCharacter* Pawn = GetOwner<AFirstPersonCharacter>())
		return Pawn->GetSensesComponent();
	else
		return nullptr;
}

