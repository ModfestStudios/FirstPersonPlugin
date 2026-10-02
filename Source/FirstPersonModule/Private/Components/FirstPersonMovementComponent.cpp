// Copyright (c) 2022 Pocket Sized Animations


#include "Components/FirstPersonMovementComponent.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "CharacterMovementComponentAsync.h"

/*components*/
#include "Components/VitalsComponent.h"

/*engine*/
#include "Engine/World.h"




UFirstPersonMovementComponent::UFirstPersonMovementComponent()
{
	MaxWalkSpeed = 280.0f;
	MaxSprintSpeed = 850.0f;

	MaxWalkSpeedCrouched = 145.0f;
	MaxCrouchSprintSpeed = 320.0f;
		
	MaxProneSpeed = 60.0f;
	MaxProneSprintSpeed = 160.0f;

	/*jump*/
	NavAgentProps.bCanJump = true;


	//MaxWalkSpeedCrouched = 

	//Crouch
	NavAgentProps.bCanCrouch = true;
	
	/*network stuff*/
	SetNetworkMoveDataContainer(CustomMoveDataContainer);
}

void UFirstPersonMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FirstPersonOwner = Cast<AFirstPersonCharacter>(GetOwner());
}

//void UFirstPersonMovementComponent::ActivateCustomMovementFlag(ECustomMovementFlags Flag)
//{
//	CustomMovementFlags |= Flag;
//}
//
//void UFirstPersonMovementComponent::ClearMovementFlag(ECustomMovementFlags Flag)
//{
//	CustomMovementFlags &= ~Flag;
//}

void UFirstPersonMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToSlowWalk = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UFirstPersonMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner);

	if (!ClientPredictionData)
	{
		UFirstPersonMovementComponent* MutableThis = const_cast<UFirstPersonMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FMultiplayerNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist =
			140.0f;
	}

	return ClientPredictionData;
}

//============================================================================================================================
//======================================================MOVEMENT UPDATES======================================================
//============================================================================================================================

void UFirstPersonMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{	
	FMultiplayerNetworkMoveData* MoveData = static_cast<FMultiplayerNetworkMoveData*>(GetCurrentNetworkMoveData());

	if (MoveData)
	{
		uint8 CustomFlags = MoveData->MoveData_CustomMovementFlags;
		bWantsToLeanLeft = (CustomFlags & CFLAG_LeanLeft) != 0;
		bWantsToLeanRight = (CustomFlags & CFLAG_LeanRight) != 0;
		bWantsToProne = (CustomFlags & CFLAG_Prone) != 0;
	}

	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

bool UFirstPersonMovementComponent::IsCustomMovementMode(ECustomMovement InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UFirstPersonMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (!CharacterOwner)
		return;

	/*
	 * Simulated proxies are applying a state received from the server.
	 *
	 * Authority and AutonomousProxy are actually performing/predicting
	 * the movement themselves.
	 */
	const bool bClientSimulation = CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy;

	const bool bWasProne =
		PreviousMovementMode == MOVE_Custom &&
		PreviousCustomMode == CUSTMOVE_Prone;

	const bool bIsNowProne =
		IsCustomMovementMode(CUSTMOVE_Prone);

	const bool bWasSliding =
		PreviousMovementMode == MOVE_Custom &&
		PreviousCustomMode == CUSTMOVE_Sliding;

	const bool bIsNowSliding =
		IsCustomMovementMode(CUSTMOVE_Sliding);


	/*========================
	 * EXIT PREVIOUS MODE
	 *========================*/

	if (bWasProne && !bIsNowProne)
	{
		ExitProne(bClientSimulation);
	}

	if (bWasSliding && !bIsNowSliding)
	{
		ExitSlide();
	}


	/*========================
	 * ENTER NEW MODE
	 *========================*/

	if (!bWasProne && bIsNowProne)
	{
		EnterProne(
			PreviousMovementMode,
			static_cast<ECustomMovement>(PreviousCustomMode),
			bClientSimulation
		);
	}

	if (!bWasSliding && bIsNowSliding)
	{
		EnterSlide(
			PreviousMovementMode,
			static_cast<ECustomMovement>(PreviousCustomMode)
		);
	}

/*
 * Authority and autonomous proxies produce/predict the actual prone state.
 *
 * Simulated proxies receive AFirstPersonCharacter::bIsProne from
 * property replication instead.
 */
	if (FirstPersonOwner && !bClientSimulation)
	{
		FirstPersonOwner->SetIsProne(bIsNowProne);
	}
}


//void UFirstPersonMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
//{
//	/*this is prior to setting the actual crouch/uncrouch state*/
//	/*double-tapping crouch*/
//	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bPrevWantsToCrouch)
//	{
//		FHitResult PotentialSlideSurface;
//		if (Velocity.SizeSquared() > pow(MinSlideSpeed, 2) && GetSlideSurface(PotentialSlideSurface))
//			SetMovementMode(MOVE_Custom, CUSTMOVE_Sliding);
//	}
//
//	/*we've let go of crouch during a slide*/
//	if (IsCustomMovementMode(CUSTMOVE_Sliding) && !bWantsToCrouch)
//		SetMovementMode(MOVE_Walking, CUSTMOVE_None);
//	/*if we're prone, and now no-longer want to be - switch to standard walking*/
//	else if (IsCustomMovementMode(CUSTMOVE_Prone) && !bWantsToProne)
//		SetMovementMode(MOVE_Walking, CUSTMOVE_None);
//
//	else if (MovementMode == MOVE_Walking && bWantsToProne)
//		SetMovementMode(MOVE_Custom, CUSTMOVE_Prone);
//
//	/*super - sets crouch & uncrouch*/
//	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
//}

void UFirstPersonMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	/*
	 * Simulated proxies receive their authoritative movement state from the server.
	 * They should NOT evaluate local bWantsTo... variables to decide whether they
	 * should enter/exit our custom movement modes.
	 *
	 * This mirrors Epic's handling of bWantsToCrouch.
	 */
	if (CharacterOwner && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		/*slide*/
		if (MovementMode == MOVE_Walking && !bWantsToCrouch && bPrevWantsToCrouch)
		{
			FHitResult PotentialSlideSurface;

			if (Velocity.SizeSquared() > FMath::Square(MinSlideSpeed) && GetSlideSurface(PotentialSlideSurface))
			{
				SetMovementMode(MOVE_Custom, CUSTMOVE_Sliding);
			}
		}

		/*leave slide*/
		if (IsCustomMovementMode(CUSTMOVE_Sliding))
		{
			if (!bWantsToCrouch)
			{
				SetMovementMode(MOVE_Walking, CUSTMOVE_None);
			}
		}

		/*prone*/
		else if (IsProne())
		{
			if (!bWantsToProne && CanExitProne())
			{
				SetMovementMode(MOVE_Walking, CUSTMOVE_None);
			}
		}

		/*enter prone*/
		else if (MovementMode == MOVE_Walking && bWantsToProne)
		{
			SetMovementMode(MOVE_Custom, CUSTMOVE_Prone);
		}
	}

	/*Epic handles native states such as crouching*/
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

/*OnMovementUpdated - Called AFTER all the movement updates*/
void UFirstPersonMovementComponent::OnMovementUpdated(float deltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(deltaSeconds, OldLocation, OldVelocity);

	/**/
	bPrevWantsToCrouch = bWantsToCrouch;

	if(!CharacterOwner || !FirstPersonOwner)
		return;

	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		FirstPersonOwner->SetIsSprinting(IsSprinting());
		FirstPersonOwner->SetIsSlowWalking(IsSlowWalking());

		if(bWantsToLeanLeft && !bWantsToLeanRight)
			FirstPersonOwner->SetLeanState(ELeanState::Left);
		else if(bWantsToLeanRight && !bWantsToLeanLeft)
			FirstPersonOwner->SetLeanState(ELeanState::Right);
		else
			FirstPersonOwner->SetLeanState(ELeanState::None);
	}
		

}

bool UFirstPersonMovementComponent::IsMovingOnGround() const
{
	/*adds customer modes as a valid "on-ground" movement modes*/
	return Super::IsMovingOnGround() || IsCustomMovementMode(CUSTMOVE_Sliding) || IsCustomMovementMode(CUSTMOVE_Prone);
}



float UFirstPersonMovementComponent::GetMaxSpeed() const
{
	if (IsProne())
	{
		if(IsSprinting())
			return MaxProneSprintSpeed;		
		if(IsSlowWalking())
			return MaxProneSlowWalkSpeed;

		return MaxProneSpeed;
	}

	else if (IsCrouching())
	{
		if(IsSprinting())
			return MaxCrouchSprintSpeed;
		if(IsSlowWalking())
			return MaxCrouchedSlowWalkSpeed;

		return MaxWalkSpeedCrouched;
	}

	else if(IsWalking())
	{		
		if (IsSprinting())
			return MaxSprintSpeed;
		if(IsSlowWalking())
			return MaxSlowWalkSpeed;

		return MaxWalkSpeed;
	}
		

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case CUSTMOVE_Sliding:
			return MaxSlideSpeed;
			break;
		default:
			return MaxWalkSpeed;
			break;
		}
	}


	return Super::GetMaxSpeed();
}

float UFirstPersonMovementComponent::GetMaxAcceleration() const
{
	if(IsProne())
		return ProneAcceleration;
	if(IsCrouching())
		return CrouchedAcceleration;

	return Super::GetMaxAcceleration();
}

void UFirstPersonMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case CUSTMOVE_Sliding:
		PhysSlide(deltaTime, Iterations);
		break;
	case CUSTMOVE_Prone:
		PhysProne(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid Movement Mode - No PhysCustom configured for mode"));
		Super::PhysCustom(deltaTime, Iterations);
		break;
		
	}

	
}

//===================================================================================================================================
//==============================================================SLIDING==============================================================
//===================================================================================================================================

void UFirstPersonMovementComponent::EnterSlide(EMovementMode PrevMode, ECustomMovement PrevCustomMode)
{
	bWantsToCrouch = true; //adjust capsule to crouch height
	Velocity += Velocity.GetSafeNormal2D() * SlideImpulseSpeed; //add our impulse	
}

void UFirstPersonMovementComponent::ExitSlide()
{
	bWantsToCrouch = false; //return capsule height

	/*reset capsule rotation*/
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
}

void UFirstPersonMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	/*safety check*/
	if (deltaTime < MIN_TICK_TIME)
		return;

	/**/
	RestorePreAdditiveRootMotionVelocity();

	/*check we can keep sliding - and exit if not*/
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(MinSlideSpeed, 2))
	{
		//ExitSlide();
		SetMovementMode(MOVE_Walking,CUSTMOVE_None);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	/*surface-gravity*/
	Velocity += SlideGravityForce * FVector::DownVector * deltaTime;

	/*slide-strafing*/
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	else
		Acceleration = FVector::ZeroVector;

	/*calculate velocity*/
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		CalcVelocity(deltaTime, SlideFriction, true, GetMaxBrakingDeceleration());

	/*add-root motion*/
	ApplyRootMotionToVelocity(deltaTime);

	/*begin move*/
	Iterations++; //+1 our iterations
	bJustTeleported = false;

	/*prep-move*/
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	//FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();

	/*move character*/
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit); //use this to actually move the capsule

	/*collision hit*/
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted); //handle smacking into something
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true); //if we hit something - we'll slide against what we hit
	}

	/*Exit slide*/
	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(MinSlideSpeed, 2))
		{
			SetMovementMode(MOVE_Walking,CUSTMOVE_None);
			ExitSlide();
		}


	/*update outgoing velocity & acceleration*/
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;

}

bool UFirstPersonMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");

	FCollisionQueryParams CollisionParams;

	/*ignore our character and all its children*/
	if (FirstPersonOwner)
	{
		TArray<AActor*> OwnerChildren;
		FirstPersonOwner->GetAllChildActors(OwnerChildren);
		CollisionParams.AddIgnoredActors(OwnerChildren);
		CollisionParams.AddIgnoredActor(FirstPersonOwner);
	}

	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, CollisionParams);
}

//=========================================================================================================================================
//================================================================CROUCHING================================================================
//=========================================================================================================================================

void UFirstPersonMovementComponent::ToggleCrouch()
{
	if (IsProne())
	{
		BeginCrouch();
		return;
	}
	
	if (IsCrouching())
		EndCrouch();
	else
		BeginCrouch();
}

void UFirstPersonMovementComponent::BeginCrouch()
{
	if (NavAgentProps.bCanCrouch)
		bWantsToCrouch = true;
	
	bWantsToProne = false;
}

void UFirstPersonMovementComponent::EndCrouch()
{
	if (NavAgentProps.bCanCrouch)
		bWantsToCrouch = false;
}

bool UFirstPersonMovementComponent::CanCrouchInCurrentState() const
{
	/*removes air-crouching*/
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

//=====================================================================================================================================
//===============================================================PRONING===============================================================
//=====================================================================================================================================

void UFirstPersonMovementComponent::ToggleProne()
{

	if(IsProne())
		EndProne();
	else
		BeginProne();
}

void UFirstPersonMovementComponent::BeginProne()
{
	//if (CanGoProne())
	bWantsToProne = true;
	bWantsToCrouch = false;
}

void UFirstPersonMovementComponent::EndProne()
{
	//if (IsCustomMovementMode(CUSTMOVE_Prone))
	bWantsToProne = false;
	bWantsToCrouch = false;
}

void UFirstPersonMovementComponent::EnterProne(EMovementMode PrevMode, ECustomMovement PrevCustomMode, bool bClientSimulation)
{
	if (!CharacterOwner || !UpdatedComponent)
		return;

	/*
	 * The slide -> prone impulse is actual gameplay movement.
	 *
	 * Authority / AutonomousProxy:
	 *     perform/predict the impulse.
	 *
	 * SimulatedProxy:
	 *     server already performed it and replicated our movement,
	 *     so do NOT apply the impulse again locally.
	 */
	if (!bClientSimulation &&
		PrevMode == MOVE_Custom &&
		PrevCustomMode == CUSTMOVE_Sliding)
	{
		Velocity += Velocity.GetSafeNormal2D() * ProneSlideEnterImpulse;
	}

	/*
	 * Prone and native crouch are mutually exclusive.
	 *
	 * Don't directly modify the replicated crouch state on a simulated
	 * proxy. The server/native crouch replication owns that state.
	 */
	if (!bClientSimulation && FirstPersonOwner && FirstPersonOwner->IsCrouched())
	{
		FirstPersonOwner->SetIsCrouched(false);
	}

	bWantsToCrouch = false;

	/*apply prone collision locally*/
	SetProneCollision(bClientSimulation);

	/*ensure movement gets an updated floor after changing capsule size*/
	bForceNextFloorCheck = true;

	FindFloor(
		UpdatedComponent->GetComponentLocation(),
		CurrentFloor,
		true,
		nullptr
	);
}

void UFirstPersonMovementComponent::ExitProne(bool bClientSimulation)
{
	if (!CharacterOwner || !UpdatedComponent)
		return;

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	if (!Capsule || !DefaultCharacter)
		return;

	const UCapsuleComponent* DefaultCapsule = DefaultCharacter->GetCapsuleComponent();

	if (!DefaultCapsule)
		return;

	/*
	 * Normal movement/prediction can use bWantsToCrouch because that
	 * desire belongs to the player performing the move.
	 *
	 * A simulated proxy must NOT make this decision from bWantsToCrouch.
	 * If crouch has replicated to this character, use the actual crouched
	 * state instead.
	 */
	const bool bExitToCrouch = bClientSimulation
		? CharacterOwner->IsCrouched()
		: bWantsToCrouch;

	const float CurrentHalfHeight =
		Capsule->GetUnscaledCapsuleHalfHeight();

	const float TargetHalfHeight =
		bExitToCrouch
		? CrouchedHalfHeight
		: DefaultCapsule->GetUnscaledCapsuleHalfHeight();

	const float TargetRadius =
		DefaultCapsule->GetUnscaledCapsuleRadius();

	const float HalfHeightAdjust =
		TargetHalfHeight - CurrentHalfHeight;

	const float ScaledHalfHeightAdjust =
		HalfHeightAdjust * Capsule->GetShapeScale();


	/*
	 * Every machine needs the proper local collision shape.
	 */
	Capsule->SetCapsuleSize(
		TargetRadius,
		TargetHalfHeight,
		true
	);


	if (!bClientSimulation)
	{
		/*
		 * Authority / autonomous prediction:
		 *
		 * Grow upward while keeping the capsule base approximately
		 * in the same place.
		 *
		 * CanExitProne() was already responsible for validating that
		 * we actually have room to perform this transition.
		 */
		UpdatedComponent->MoveComponent(
			-ScaledHalfHeightAdjust * GetGravityDirection(),
			UpdatedComponent->GetComponentQuat(),
			true
		);
	}
	else
	{
		/*
		 * Simulated proxy:
		 *
		 * Do not independently reposition the actor. Its authoritative
		 * transform comes from the server.
		 */
		bShrinkProxyCapsule = true;
	}


	AdjustProxyCapsuleSize();

	bForceNextFloorCheck = true;

	FindFloor(
		UpdatedComponent->GetComponentLocation(),
		CurrentFloor,
		true,
		nullptr
	);
}



bool UFirstPersonMovementComponent::CanGoProne() const
{
	if (MovementMode == MOVE_Walking)
	{
		/*if (bWantsToCrouch == true)
			return false;	*/

		return true;
	}


	return false;
}

bool UFirstPersonMovementComponent::CanExitProne() const
{
	if (!CharacterOwner || !UpdatedComponent)
		return false;

	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	if (!Capsule || !DefaultCharacter)
		return false;

	const UCapsuleComponent* DefaultCapsule = DefaultCharacter->GetCapsuleComponent();
	if (!DefaultCapsule)
		return false;

	const bool bExitToCrouch = bWantsToCrouch;

	const float CapsuleScale = Capsule->GetShapeScale();

	const float TargetHalfHeight = bExitToCrouch
		? CrouchedHalfHeight * CapsuleScale
		: DefaultCapsule->GetUnscaledCapsuleHalfHeight() * CapsuleScale;

	const float CurrentHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float TargetRadius = DefaultCapsule->GetUnscaledCapsuleRadius() * CapsuleScale;

	const float HalfHeightAdjust = TargetHalfHeight - CurrentHalfHeight;

	const FVector TestLocation =
		UpdatedComponent->GetComponentLocation() +
		FVector::UpVector * HalfHeightAdjust;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CanExitProne), false, CharacterOwner);

	return !GetWorld()->OverlapBlockingTestByChannel(
		TestLocation,
		UpdatedComponent->GetComponentQuat(),
		UpdatedComponent->GetCollisionObjectType(),
		FCollisionShape::MakeCapsule(TargetRadius, TargetHalfHeight),
		Params
	);
}

bool UFirstPersonMovementComponent::IsProne() const
{
	return IsCustomMovementMode(CUSTMOVE_Prone);	
}

void UFirstPersonMovementComponent::PhysProne(float deltaTime, int32 Iterations)
{
	/*safety check*/
	if (deltaTime < MIN_TICK_TIME)
		return;

	/*zero-out acceleration/velocity if character or dependency is invalid*/
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	/*prep*/
	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	/*perform move*/
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;

		/*get time-stepping*/
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		/*subtract from remaining time*/
		remainingTime -= timeTick;

		/*get current value/state incase we need to revert*/
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		/*ensure velocity is horizontal*/
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration.Z = 0.0f; //zero out Z (up) acceleration

		/*apply acecleration*/
		CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration()); //helps apply friction/braking

		/*compute move parameters*/
		const FVector MoveVelocity = Velocity; //direction & speed
		const FVector Delta = timeTick * MoveVelocity; //target move
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		/*clear remaining time if our delta hit zero (or close enough)*/
		if (bZeroDelta)
			remainingTime = 0.0f;
		else
		{
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			/**/
			if (IsFalling())
			{
				/*player jumped?*/
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.0f, ActualDist / DesiredDist));
				}
				/*switch to new physics */
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			/*hit water*/
			else if (IsSwimming())
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		/*walking off ledges*/
		if (StepDownResult.bComputedFloor)
			CurrentFloor = StepDownResult.FloorResult;
		else
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);

		/*check for ledges*/
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			/*calculate possible alternative movement*/
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			//const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, CurrentFloor);

			if (!NewDelta.IsNearlyZero())
			{
				/*revert first move*/
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				/*avoid repeated ledge movement*/
				bTriedLedgeMove = true;

				/*try new movement direction*/
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
		// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			/*we have a valid floor*/
			if (CurrentFloor.IsWalkableFloor())
			{
				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.GetEvenIfUnreachable(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			/*just enterted water*/
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			/*check to see if we need to start falling*/
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

void UFirstPersonMovementComponent::SetProneCollision(bool bClientSimulation)
{
	if (!CharacterOwner || !UpdatedComponent)
		return;

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	if (!Capsule)
		return;

	const float OldHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float Radius = Capsule->GetUnscaledCapsuleRadius();

	/*capsule half-height can never be less than its radius*/
	const float NewHalfHeight = FMath::Max(ProneHalfHeight, Radius);

	if (FMath::IsNearlyEqual(OldHalfHeight, NewHalfHeight))
		return;

	const float HalfHeightAdjust = OldHalfHeight - NewHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * Capsule->GetShapeScale();

	/*
	 * Both normal movement and simulated proxies need the correct
	 * physical capsule dimensions locally.
	 */
	Capsule->SetCapsuleSize(
		Radius,
		NewHalfHeight,
		true
	);

	if (!bClientSimulation)
	{
		/*
		 * We are actually performing/predicting the prone transition.
		 *
		 * Move the capsule center downward by the amount removed from
		 * its half-height so the bottom of the capsule stays approximately
		 * in the same location.
		 */
		UpdatedComponent->MoveComponent(
			ScaledHalfHeightAdjust * GetGravityDirection(),
			UpdatedComponent->GetComponentQuat(),
			true
		);
	}
	else
	{
		/*
		 * Simulated proxy:
		 *
		 * Do NOT independently reposition the actor.
		 * The authoritative server transform is already being replicated.
		 *
		 * Tell CharacterMovement to perform its normal proxy-capsule
		 * shrink handling.
		 */
		bShrinkProxyCapsule = true;
	}

	AdjustProxyCapsuleSize();

	bForceNextFloorCheck = true;
}


//==========================================================
//=======================SLOW-WALKING=======================
//==========================================================

void UFirstPersonMovementComponent::BeginSlowWalk()
{
	bWantsToSlowWalk = true;
	bWantsToSprint = false;
}

void UFirstPersonMovementComponent::EndSlowWalk()
{
	bWantsToSlowWalk = false;
}

bool UFirstPersonMovementComponent::IsSlowWalking() const
{
	return bWantsToSlowWalk && CanSlowWalk();
}

bool UFirstPersonMovementComponent::CanSlowWalk() const
{
	if(MovementMode != MOVE_Walking && !IsCustomMovementMode(CUSTMOVE_Prone))
		return false;

	if(Acceleration.IsNearlyZero())
		return false;

	return true;
}

//=========================================================
//========================SPRINTING========================
//=========================================================
void UFirstPersonMovementComponent::BeginSprint()
{
	bWantsToSprint = true;
	bWantsToSlowWalk = false;
}

void UFirstPersonMovementComponent::EndSprint()
{
	bWantsToSprint = false;
}

bool UFirstPersonMovementComponent::CanSprint() const
{	
	/*checks to see if we're either walk/crouching or prone*/
	if((MovementMode != MOVE_Walking && !IsCustomMovementMode(CUSTMOVE_Prone))) //walk/crouch are bundled into MOVE_WALKINg while prone is its own CUSTMOVE_Prone
		return false;	


	if(Acceleration.IsNearlyZero())
		return false;

		
	/*ensure we're moving forward*/
	const FVector MoveDirection = Acceleration.GetSafeNormal2D();
	const FVector ForwardDirection = UpdatedComponent->GetForwardVector().GetSafeNormal2D();

	return FVector::DotProduct(MoveDirection, ForwardDirection) > 0.5f;	
}

bool UFirstPersonMovementComponent::IsSprinting() const
{
	return bWantsToSprint && CanSprint();
}

//void UFirstPersonMovementComponent::ToggleSprintingFlag(bool bShouldSprint)
//{
//	bShouldSprint ? ActivateCustomMovementFlag(ECustomMovementFlags::CFLAG_Sprint) : ClearMovementFlag(ECustomMovementFlags::CFLAG_Sprint);
//
//}

void UFirstPersonMovementComponent::BeginLeanLeft()
{
	bWantsToLeanLeft = true;
	EndLeanRight();
}

void UFirstPersonMovementComponent::EndLeanLeft()
{
	bWantsToLeanLeft = false;
}

void UFirstPersonMovementComponent::BeginLeanRight()
{
	bWantsToLeanRight = true;
	EndLeanLeft();	
}

void UFirstPersonMovementComponent::EndLeanRight()
{
	bWantsToLeanRight = false;
}

void UFirstPersonMovementComponent::SetCapsuleHeight(float NewHeight)
{
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedProneHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, NewHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedProneHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedProneHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
}




//===========================================================================================================================================================================
//=============================================================================NETWORK MOVE DATA=============================================================================
//===========================================================================================================================================================================
void UFirstPersonMovementComponent::FMultiplayerNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FMultiplayerSavedMove& SavedMove = static_cast<const FMultiplayerSavedMove&>(ClientMove);
	MoveData_CustomMovementFlags = SavedMove.GetCustomMovementFlags(); //build our custom packed byte to be sent
	
	//MoveData_CustomMovementFlags = SavedMove.Saved_CustomMovementFlags;

}

bool UFirstPersonMovementComponent::FMultiplayerNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	const bool bSuperSuccess = Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);
	const bool bIsSaving = Ar.IsSaving();

	SerializeOptionalValue<uint8>(bIsSaving,Ar,MoveData_CustomMovementFlags,0);

	return bSuperSuccess && !Ar.IsError();
}

UFirstPersonMovementComponent::FMultiplayerNetworkMoveDataContainer::FMultiplayerNetworkMoveDataContainer()
{
	NewMoveData = &CustomDefaultMoveData[0];
	PendingMoveData = &CustomDefaultMoveData[1];
	OldMoveData = &CustomDefaultMoveData[2];
}

void UFirstPersonMovementComponent::FMultiplayerSavedMove::Clear()
{
	Super::Clear();

	Saved_bPrevWantsToCrouch = 0;
	Saved_bWantsToSlowWalk = 0;
	Saved_bWantsToSprint = 0;
	Saved_bWantsToLeanLeft = 0;
	Saved_bWantsToLeanRight = 0;	
	Saved_bWantsToProne = 0;

	//Saved_CustomMovementFlags = 0; //@todo: remove - split out custom movement into individuals
}

uint8 UFirstPersonMovementComponent::FMultiplayerSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint)
		Result |= FLAG_Custom_0;
	if(Saved_bWantsToSlowWalk)
		Result |= FLAG_Custom_1;

	//if(bLeaningLeft)

	return Result;
}

uint8 UFirstPersonMovementComponent::FMultiplayerSavedMove::GetCustomMovementFlags() const
{
	uint8 Result = 0;

	if(Saved_bWantsToLeanLeft)
		Result |= CFLAG_LeanLeft;
	if(Saved_bWantsToLeanRight)
		Result |= CFLAG_LeanRight;
	if(Saved_bWantsToProne)
		Result |= CFLAG_Prone;

	return Result;
}

bool UFirstPersonMovementComponent::FMultiplayerSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	/*cast from UE's SavedMove to our Custom SavedMove*/
	FMultiplayerSavedMove* NewMovePtr = static_cast<FMultiplayerSavedMove*>(NewMove.Get());

	/*if our custom movement flags are not the same - then we cannot combine (automatically compares all movement flags)*/
	if(GetCustomMovementFlags() != NewMovePtr->GetCustomMovementFlags())
		return false;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}


/*SetMoveFor() - Function that will look at the Character Movment INSTANCE and push ITS variables into the SavedMovement data to be sent over the network
* Packing the data to be applied by the server after being sent
* Here we want to CONSTRUCT the movement data by setting our custom variables into the "SnapShot" of movement
* Note: This just configures the SavedMove, the SavedMove still needs to compress these into Network-Sent Variables in a lightweight 'compressed' packet (these are not sent directly)
*
*/
void UFirstPersonMovementComponent::FMultiplayerSavedMove::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);


	if (UFirstPersonMovementComponent* CMC = Cast<UFirstPersonMovementComponent>(C->GetCharacterMovement()))
	{
		Saved_bWantsToSprint = CMC->bWantsToSprint;
		Saved_bWantsToSlowWalk = CMC->bWantsToSlowWalk;
		Saved_bWantsToLeanLeft = CMC->bWantsToLeanLeft;
		Saved_bWantsToLeanRight = CMC->bWantsToLeanRight;		
		Saved_bPrevWantsToCrouch = CMC->bPrevWantsToCrouch;
		Saved_bWantsToProne = CMC->bWantsToProne;

		//Saved_CustomMovementFlags = CMC->CustomMovementFlags; //we use to set custom flags as packed already - but we now set the custom flags individually for consistency
	}

}

/*PrepMoveFor() - Opposite of SetMoveFor(). Takes the SavedMove variables set and applies them to the Character Movement Instance (unpacking the data and applying it)*/
void UFirstPersonMovementComponent::FMultiplayerSavedMove::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UFirstPersonMovementComponent* CMC = Cast<UFirstPersonMovementComponent>(C->GetCharacterMovement()))
	{
		CMC->bWantsToSprint = Saved_bWantsToSprint;
		CMC->bWantsToSlowWalk = Saved_bWantsToSlowWalk;
		CMC->bWantsToLeanLeft = Saved_bWantsToLeanLeft;
		CMC->bWantsToLeanRight = Saved_bWantsToLeanRight;
		CMC->bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
		CMC->bWantsToProne = Saved_bWantsToProne;

		//CMC->CustomMovementFlags = Saved_CustomMovementFlags; //we use to set custom flags as packed already - but we now set the custom flags individually for consistency
	}
}

UFirstPersonMovementComponent::FMultiplayerNetworkPredictionData_Client::FMultiplayerNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{

}

FSavedMovePtr UFirstPersonMovementComponent::FMultiplayerNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FMultiplayerSavedMove());
}
