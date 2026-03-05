// Copyright (c) 2022 Pocket Sized Animations


#include "Components/FirstPersonMovementComponent.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "CharacterMovementComponentAsync.h"


/*engine*/
#include "Engine/World.h"




UFirstPersonMovementComponent::UFirstPersonMovementComponent()
{
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

void UFirstPersonMovementComponent::ActivateCustomMovementFlag(ECustomMovementFlags Flag)
{
	CustomMovementFlags |= Flag;
}

void UFirstPersonMovementComponent::ClearMovementFlag(ECustomMovementFlags Flag)
{
	CustomMovementFlags &= ~Flag;
}

void UFirstPersonMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

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
	if (MoveData != nullptr)
	{
		CustomMovementFlags = MoveData->MoveData_CustomMovementFlags;
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

	/*custom moves*/
	if (PreviousMovementMode == MOVE_Custom)
	{
		switch (PreviousCustomMode)
		{
		case CUSTMOVE_Prone:
			ExitProne();
			break;
		case CUSTMOVE_Sliding:
			ExitSlide();
			break;
		}
	}

	if (IsCustomMovementMode(CUSTMOVE_Prone))
		EnterProne(PreviousMovementMode, (ECustomMovement)PreviousCustomMode);
	if (IsCustomMovementMode(CUSTMOVE_Sliding))
		EnterSlide(PreviousMovementMode, (ECustomMovement)PreviousCustomMode);
}


void UFirstPersonMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	/*this is prior to setting the actual crouch/uncrouch state*/
	/*double-tapping crouch*/
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > pow(MinSlideSpeed, 2) && GetSlideSurface(PotentialSlideSurface))
			SetMovementMode(MOVE_Custom, CUSTMOVE_Sliding);
	}

	/*we've let go of crouch during a slide*/
	if (IsCustomMovementMode(CUSTMOVE_Sliding) && !bWantsToCrouch)
		SetMovementMode(MOVE_Walking, CUSTMOVE_None);
	else if (IsCustomMovementMode(CUSTMOVE_Prone))
		SetMovementMode(MOVE_Walking, CUSTMOVE_None);

	else if (MovementMode == MOVE_Walking && bWantsToProne)
		SetMovementMode(MOVE_Custom, CUSTMOVE_Prone);

	/*super - sets crouch & uncrouch*/
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

/*OnMovementUpdated - Called AFTER all the movement updates*/
void UFirstPersonMovementComponent::OnMovementUpdated(float deltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(deltaSeconds, OldLocation, OldVelocity);

	/**/
	bPrevWantsToCrouch = bWantsToCrouch;
}

bool UFirstPersonMovementComponent::IsMovingOnGround() const
{
	/*adds customer modes as a valid "on-ground" movement modes*/
	return Super::IsMovingOnGround() || IsCustomMovementMode(CUSTMOVE_Sliding) || IsCustomMovementMode(CUSTMOVE_Prone);
}



float UFirstPersonMovementComponent::GetMaxSpeed() const
{
	//if (IsCustomFlagSet(ECustomMovementFlags::CFLAG_Sprint))

	/*sprinting*/
	if (MovementMode == MOVE_Walking && bSprinting && !IsCrouching())
		return MaxSprintSpeed;

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case CUSTMOVE_Prone:
			return ProneMaxSpeed;
			break;
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

void UFirstPersonMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CUSTMOVE_Sliding:
		PhysSlide(deltaTime, Iterations);
		break;
	case CUSTMOVE_Prone:
		PhysProne(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode - No PhysCustom configured for mode"));
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
		ExitSlide();
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
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
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
		ExitSlide();

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
	if (bWantsToCrouch)
		EndCrouch();
	else
		BeginCrouch();
}

void UFirstPersonMovementComponent::BeginCrouch()
{
	if (NavAgentProps.bCanCrouch)
		bWantsToCrouch = true;
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
	if (bWantsToProne)
		EndProne();
	else
		BeginProne();
}

void UFirstPersonMovementComponent::BeginProne()
{
	if (CanGoProne())
		bWantsToProne = true;
}

void UFirstPersonMovementComponent::EndProne()
{
	if (IsCustomMovementMode(CUSTMOVE_Prone))
		bWantsToProne = false;
}

void UFirstPersonMovementComponent::EnterProne(EMovementMode PrevMode, ECustomMovement PrevCustomMode)
{
	/*allows for a little cool 'dive' impulse if we were sliding*/
	if (PrevMode == MOVE_Custom && PrevCustomMode == CUSTMOVE_Sliding)
		Velocity += Velocity.GetSafeNormal2D() * ProneSlideEnterImpulse;

	/*if we were previously crouching*/
	if (PrevMode == MOVE_Walking && bWantsToCrouch)
	{
		/*reset crouching desire so not to confuse life*/
		bWantsToCrouch = false;
	}


	/*setup collision capsule size*/
	//SetCapsuleHeight(ProneHalfHeight);


	/*ensure we have a floor/ground under us*/
	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
}

void UFirstPersonMovementComponent::ExitProne()
{
	//nothing here yet!
	//SetCapsuleHeight()
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

void UFirstPersonMovementComponent::BeginSprint()
{
	bSprinting = true;
}

void UFirstPersonMovementComponent::EndSprint()
{
	bSprinting = false;
}

void UFirstPersonMovementComponent::ToggleSprintingFlag(bool bShouldSprint)
{
	bShouldSprint ? ActivateCustomMovementFlag(ECustomMovementFlags::CFLAG_Sprint) : ClearMovementFlag(ECustomMovementFlags::CFLAG_Sprint);

}

void UFirstPersonMovementComponent::LeanLeft()
{

}

void UFirstPersonMovementComponent::LeanRight()
{

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
	MoveData_CustomMovementFlags = SavedMove.Saved_CustomMovementFlags;
}

bool UFirstPersonMovementComponent::FMultiplayerNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	const bool bIsSaving = Ar.IsSaving();

	SerializeOptionalValue<uint8>(bIsSaving, Ar, MoveData_CustomMovementFlags, 0);

	return !Ar.IsError();
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

	Saved_bWantsToSprint = 0;
	Saved_bWantsToLeanLeft = 0;
	Saved_bWantsToLeanRight = 0;
	Saved_CustomMovementFlags = 0;
	Saved_bWantsToProne = 0;
}

uint8 UFirstPersonMovementComponent::FMultiplayerSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint)
		Result |= FLAG_Custom_0;


	//if(bLeaningLeft)

	return Result;
}

bool UFirstPersonMovementComponent::FMultiplayerSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	/*cast from UE's SavedMove to our Custom SavedMove*/
	FMultiplayerSavedMove* NewMovePtr = static_cast<FMultiplayerSavedMove*>(NewMove.Get());

	//begin checking custom flags

	/*check if our Movement Flags are equal*/
	if (Saved_CustomMovementFlags != NewMovePtr->Saved_CustomMovementFlags)
	{
		return false;
	}

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
		Saved_bWantsToSprint = CMC->bSprinting;
		Saved_CustomMovementFlags = CMC->CustomMovementFlags;
		Saved_bPrevWantsToCrouch = CMC->bPrevWantsToCrouch;
		Saved_bWantsToProne = CMC->bWantsToProne;
	}

}

/*PrepMoveFor() - Opposite of SetMoveFor(). Takes the SavedMove variables set and applies them to the Character Movement Instance (unpacking the data and applying it)*/
void UFirstPersonMovementComponent::FMultiplayerSavedMove::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UFirstPersonMovementComponent* CMC = Cast<UFirstPersonMovementComponent>(C->GetCharacterMovement()))
	{
		CMC->bSprinting = Saved_bWantsToSprint;
		CMC->CustomMovementFlags = Saved_CustomMovementFlags;
		CMC->bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
		CMC->bWantsToProne = Saved_bWantsToProne;
	}
}

UFirstPersonMovementComponent::FMultiplayerNetworkPredictionData_Client::FMultiplayerNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{

}

FSavedMovePtr UFirstPersonMovementComponent::FMultiplayerNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FMultiplayerSavedMove());
}
