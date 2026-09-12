// Copyrighted : Modfest Studios 2025-2026


#include "Subsystems/CombatSubsystem.h"
#include "Combat/Combat.h"
#include "Combat/AttackAsset.h"

/*debug*/
#include "DrawDebugHelpers.h"


void UCombatSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*process our remaining attacks*/
	if(Attacks.Num() > 0)
		ProcessAttacks(DeltaTime);
	

}

//void UCombatSubsystem::ProcessAttacks(float DeltaTime)
//{
//	/*loop backwards so attacks can safely be removed*/
//	for (int32 AttackIndex = Attacks.Num() - 1; AttackIndex >= 0; --AttackIndex)
//	{
//		FActiveAttack& Attack = Attacks[AttackIndex];
//
//		/*safety check*/
//		if (!IsValid(Attack.AttackAsset) || !IsValid(Attack.Attacker))
//		{
//			RemoveAttack(AttackIndex);
//			continue;
//		}
//
//		/*advance attack time*/
//		Attack.ElapsedTime += DeltaTime;
//
//		/*attack has completely finished*/
//		if (Attack.ElapsedTime >= Attack.EndTime)
//		{
//			RemoveAttack(AttackIndex);
//			continue;
//		}
//
//		/*cache values used by all hit volumes*/
//		AActor* Attacker = Attack.Attacker;
//		const FTransform ActorTransform = Attacker->GetActorTransform();
//		const float ElapsedTime = Attack.ElapsedTime;
//
//		/*process active hit volumes*/
//		for (const FAttackHitVolume& Volume : Attack.HitVolumes)
//		{
//			const float StartTime = Volume.StartDelay;
//			const float EndTime = Volume.StartDelay + Volume.Duration;
//
//			/*volume isn't active right now*/
//			if (ElapsedTime < StartTime || ElapsedTime >= EndTime)
//				continue;
//
//			const FVector SweepStart = ActorTransform.TransformPosition(Volume.StartOffset);
//			const FVector SweepDirection = Volume.Direction;
//			const float SweepRange = Volume.Range;
//			const EAttackTraceShape TraceShape = Volume.TraceShape;
//			const FVector TraceExtent = Volume.ShapeExtent;
//			const FRotator ShapeRotation = Volume.ShapeRotation;
//
//			TArray<AActor*> IgnoredActors;
//
//			PerformMeleeTrace(SweepStart,SweepDirection,SweepRange,TraceShape,TraceExtent,ShapeRotation,Attacker,IgnoredActors,Attack.bDebugging
//			);
//		}
//	}
//}

void UCombatSubsystem::ProcessAttacks(float DeltaTime)
{
	/*loop backwards so we can safely remove attacks*/
	for (int32 AttackIndex = Attacks.Num() - 1; AttackIndex >= 0; --AttackIndex)
	{
		FActiveAttack& Attack = Attacks[AttackIndex];

		/*safety check*/
		if (!IsValid(Attack.AttackAsset) || !IsValid(Attack.Attacker))
		{
			RemoveAttack(AttackIndex);
			continue;
		}

		Attack.ElapsedTime += DeltaTime;

		AActor* Attacker = Attack.Attacker;
		const FTransform ActorTransform = Attacker->GetActorTransform();

		bool bAttackFinished = true;

		for (FActiveAttackVolume& ActiveVolume : Attack.HitVolumes)
		{
			const FAttackHitVolume& Volume = ActiveVolume.Volume;

			/*waiting for activation*/
			if (!ActiveVolume.bActivated)
			{
				bAttackFinished = false;
				continue;
			}

			const float StartTime = ActiveVolume.ActivationTime;
			const float EndTime = StartTime + Volume.Duration;

			/*volume still has lifetime remaining*/
			if (Attack.ElapsedTime < EndTime)
			{
				bAttackFinished = false;
			}

			/*not currently inside this volume's active window*/
			if (Attack.ElapsedTime < StartTime || Attack.ElapsedTime >= EndTime)
			{
				continue;
			}

			const FVector SweepStart = Attacker->GetActorLocation() + Attacker->GetActorQuat().RotateVector(Volume.StartOffset);
			const FVector SweepDirection =
				ActorTransform.TransformVectorNoScale(Volume.Direction).GetSafeNormal();

			const FQuat WorldShapeRotation =
				Attacker->GetActorQuat() * Volume.ShapeRotation.Quaternion();

			const FRotator ShapeRotation = WorldShapeRotation.Rotator();

			TArray<AActor*> IgnoredActors;

			const TArray<FHitResult> HitResults = PerformMeleeTrace(
				SweepStart,
				SweepDirection,
				Volume.Range,
				Volume.TraceShape,
				Volume.ShapeExtent,
				ShapeRotation,
				Attacker,
				IgnoredActors,
				Attack.bDebugging
			);

			/*process hits here*/
		}

		if (bAttackFinished)
		{
			RemoveAttack(AttackIndex);
		}
	}
}

void UCombatSubsystem::AddAttack(UAttackAsset* Attack, AActor* Attacker)
{
	if (!Attack || !Attacker)
		return;

	FActiveAttack NewAttack;

	NewAttack.Attacker = Attacker;
	NewAttack.AttackAsset = Attack;
	NewAttack.ExecutionTimestamp = GetWorld()->GetTimeSeconds();
	NewAttack.ElapsedTime = 0.0f;
	NewAttack.bDebugging = Attack->bDebugAttack;

	for (const FAttackHitVolume& Volume : Attack->HitVolumes)
	{
		FActiveAttackVolume ActiveVolume;

		ActiveVolume.Volume = Volume;

		/*timed volumes are effectively scheduled from attack start*/
		if (Volume.AttackTiming == EAttackTiming::Timed)
		{
			ActiveVolume.bActivated = true;
			ActiveVolume.ActivationTime = Volume.StartDelay;
		}

		NewAttack.HitVolumes.Add(ActiveVolume);
	}

	Attacks.Add(MoveTemp(NewAttack));
}

void UCombatSubsystem::RemoveAttack(int32 AttackIndex)
{
	if(Attacks.IsValidIndex(AttackIndex))
		Attacks.RemoveAtSwap(AttackIndex);
}

void UCombatSubsystem::InitiateAttack(UAttackAsset* Attack, class AActor* Attacker)
{
	/*make sure we have a valid attack to process*/
	if(!Attack || !Attacker)
		return;

	AddAttack(Attack, Attacker);
}

void UCombatSubsystem::ActivateAttackVolume(AActor* Attacker, FName VolumeName)
{
	if (!IsValid(Attacker) || VolumeName.IsNone())
		return;

	for (FActiveAttack& Attack : Attacks)
	{
		if (Attack.Attacker != Attacker)
			continue;

		for (FActiveAttackVolume& ActiveVolume : Attack.HitVolumes)
		{
			/*make sure this is the requested volume*/
			if (ActiveVolume.Volume.VolumeName != VolumeName)
				continue;

			/*only AnimEvent volumes should be manually activated*/
			if (ActiveVolume.Volume.AttackTiming != EAttackTiming::AnimEvent)
				continue;

			/*don't restart an already active/used volume*/
			if (ActiveVolume.bActivated)
				return;

			ActiveVolume.bActivated = true;
			ActiveVolume.ActivationTime = Attack.ElapsedTime;

			return;
		}
	}
}

TArray<FHitResult> UCombatSubsystem::PerformMeleeTrace(FVector TraceStart, FVector Direction, float Range, EAttackTraceShape TraceShape, FVector ShapeExtent, FRotator ShapeRotation, AActor* Instigator, const TArray<AActor*>& IgnoredActors, bool bDebug)
{
	/*initialize*/
	TArray<FHitResult> HitResults;
	ShapeExtent = ShapeExtent.GetAbs(); //santiization
	Direction = Direction.GetSafeNormal(); //normalize just in-case it's not 0-1 unit range
	FVector TraceEnd = TraceStart + (Direction * Range);
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel3; //designated melee channel
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(PerformMeleeTrace), false);
	CollisionParams.AddIgnoredActor(Instigator);		
	CollisionParams.AddIgnoredActors(IgnoredActors);
	bool bBlockingHit = false;	
	FCollisionShape CollisionShape;

	switch (TraceShape)
	{
	case EAttackTraceShape::Box:
		CollisionShape = FCollisionShape::MakeBox(ShapeExtent);
		break;

	case EAttackTraceShape::Sphere:
		CollisionShape = FCollisionShape::MakeSphere(ShapeExtent.X);
		break;

	case EAttackTraceShape::Capsule:
		CollisionShape = FCollisionShape::MakeCapsule(ShapeExtent.X, ShapeExtent.Z);
		break;

	default:
		CollisionShape = FCollisionShape::MakeBox(ShapeExtent);
		break;
	}

	if(bDebug)
		UE_LOG(LogTemp, Log, TEXT("UCombatSubsystem::PerformMeleeTrace()"));

	/*perform trace*/
	bBlockingHit = GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, ShapeRotation.Quaternion(), TraceChannel, CollisionShape, CollisionParams);
	
	
	if (bDebug)
	{
		/*line to show direction*/
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 2.0f,1,3.0f);	

		for (FHitResult Hit : HitResults)
		{
			if(Hit.GetActor())
				UE_LOG(LogTemp,Log,TEXT("\t HIT ACTOR (%s)"), *GetNameSafe(Hit.GetActor()));
			if(Hit.GetComponent())
				UE_LOG(LogTemp, Log, TEXT("\t HIT COMPONENT (%s)"), *GetNameSafe(Hit.GetComponent()));
		}

		FColor InactiveColor = FColor::Silver;
		FColor StartColor = FColor::Turquoise;
		FColor HitColor = FColor::Orange;
		FColor MissedColor = FColor::Yellow;
		bool bHit = HitResults.Num() > 0;

		if (TraceShape == EAttackTraceShape::Box)
		{
			DrawDebugBox(
				GetWorld(),
				TraceStart,
				ShapeExtent,
				StartColor,
				false,
				2.0f);
			
			DrawDebugBox(GetWorld(),
			TraceEnd,
			ShapeExtent,
			ShapeRotation.Quaternion(),
			bHit ? HitColor : MissedColor, 
			false, bHit ? 4.0f : 2.0f,
			bHit ? 1 : 2, 
			bHit ? 3.0f : 1.0f);
		}
		else if (TraceShape == EAttackTraceShape::Capsule)
		{
			DrawDebugCapsule(
				GetWorld(),
				TraceStart,
				ShapeExtent.Z,
				ShapeExtent.X,
				ShapeRotation.Quaternion(),
				StartColor,
				false,
				2.0f
			);

			DrawDebugCapsule(
				GetWorld(),
				TraceEnd,
				ShapeExtent.Z,
				ShapeExtent.X,
				ShapeRotation.Quaternion(),
				bHit ? HitColor : MissedColor,
				false,
				bHit ? 4.0f : 2.0f,
				bHit ? 1 : 2,
				bHit ? 3.0f : 1.0f
			);
		}
		else if (TraceShape == EAttackTraceShape::Sphere)
		{
			DrawDebugSphere(
				GetWorld(),
				TraceStart,
				ShapeExtent.X,
				12,
				StartColor,
				false,
				2.0f
			);

			DrawDebugSphere(
				GetWorld(),
				TraceEnd,
				ShapeExtent.X,
				12,
				bHit ? HitColor : MissedColor,
				false,
				bHit ? 4.0f : 2.0f,
				bHit ? 1 : 2,
				bHit ? 3.0f : 1.0f
			);
		}
	}

	/*return results*/
	return HitResults;
}
