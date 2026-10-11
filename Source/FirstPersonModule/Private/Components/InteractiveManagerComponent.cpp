// Copyright (c) 2022 Pocket Sized Animations


#include "Components/InteractiveManagerComponent.h"

/*components*/
#include "Components/InteractiveComponent.h"
#include "Components/InteractiveCollisionComponent.h"

/*camera*/
#include "Camera/CameraComponent.h"

/*interfaces*/
#include "Interfaces/InteractiveShapeInterface.h"

/*engine*/
#include "Engine/World.h"

/*utilities*/
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UInteractiveManagerComponent::UInteractiveManagerComponent()
{
	TraceLength = 140.0f;
	TraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);


	/*tick setup*/
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.15f;
}


void UInteractiveManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}



void UInteractiveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	/*perform trace to see what's infront of us*/
	TraceForInteractives();

	/*determine which one we're focused on*/
	CalcFocusedInteractive();

	/*calculate what's usable*/
	CalcInteractives();

	/**/
	UpdateActiveInteractiveStatus();
}

//================================================
//==================INTERACTIONS==================
//================================================

UInteractiveComponent* UInteractiveManagerComponent::GetFocusedInteractive()
{
	return FocusedInteractive;
}

void UInteractiveManagerComponent::StartInteraction(UInteractiveComponent* Interactive, UInteractiveAction* Action)
{
	if(!Interactive || !GetOwner())
		return;

	/*simple notifier for when a user tries to do something that isn't allowed - we don't CANCEL the interaction because we want the interactive itself to provide feedback*/
	if (!Interactive->IsActionAvailable(GetCharacterOwner(), Action))
	{		
		NativeOnInteractionDenied(Interactive, Action);
		return; //stop the itneraction here
	}
		
	/*perform interaction*/	
	if (GetOwner()->HasAuthority()) /*server*/
	{
		/*notify the interactive it was being used*/
		Interactive->NativeOnBeginInteraction(GetCharacterOwner(), Action);
	}
	else /*clients*/
	{
		/*request server to run the interaction*/
		const FGuid& InteractiveID = Action->ID;
		Server_StartInteraction(Interactive, InteractiveID);
	}
}

void UInteractiveManagerComponent::StopInteraction(UInteractiveComponent* Interactive, UInteractiveAction* Action, bool bWasInterrupted)
{
	if(!Interactive)
		return;

	EInteractionEndReason EndReason = bWasInterrupted ? EInteractionEndReason::Interrupted : EInteractionEndReason::Canceled;
	Interactive->NativeOnStopInteraction(GetCharacterOwner(), Action, EndReason);	
}


bool UInteractiveManagerComponent::Server_StartInteraction_Validate(UInteractiveComponent* Interactive, const FGuid& ID)
{
	return true;
}

void UInteractiveManagerComponent::Server_StartInteraction_Implementation(UInteractiveComponent* Interactive, const FGuid& ID)
{
	if(!Interactive || !ID.IsValid())
		return;
	
	UInteractiveAction* Action = Interactive->GetActionByID(ID);
	StartInteraction(Interactive,Action);
}

bool UInteractiveManagerComponent::Server_StopInteraction_Validate(UInteractiveComponent* Interactive, const FGuid& ID)
{
	return true;
}

void UInteractiveManagerComponent::Server_StopInteraction_Implementation(UInteractiveComponent* Interactive, const FGuid& ID)
{
	if(!Interactive || !ID.IsValid())
		return;

	UInteractiveAction* Action = Interactive->GetActionByID(ID);
	StopInteraction(Interactive, Action);
}




/*NativeOnInteractionStarted() - Event Hook called by the Interactive Component when the actual interaction has started
* @NOTE: should only be used to help manage the state of the manager - not drive logic
*
*
**/

void UInteractiveManagerComponent::NativeOnInteractionStarted(UInteractiveComponent* Interactive, const UInteractiveAction* Action)
{
	ActiveInteractive = Interactive;
	ActiveAction = const_cast<UInteractiveAction*>(Action);
	InteractionStartTime = GetWorld()->GetTimeSeconds();
}

void UInteractiveManagerComponent::NativeOnInteractionCompleted(UInteractiveComponent* Interactive, const UInteractiveAction* Action)
{
	InteractionStartTime = 0.0f;	
}

void UInteractiveManagerComponent::NativeOnInteractionCancelled(UInteractiveComponent* Interactive, const UInteractiveAction* Action, bool bInterrupted)
{
	InteractionStartTime = 0.0f;
}

void UInteractiveManagerComponent::NativeOnInteractionDenied(UInteractiveComponent* Interactive, UInteractiveAction* Action)
{
	//if (OnInteractionDenied.IsBound())
	//	OnInteractionDenied.Broadcast(DeniedActor, GetOwner());
}


//=====================================
//===============ACTIONS===============
//=====================================

TArray<UInteractiveAction*> UInteractiveManagerComponent::GetFocusedInteractiveActions() const
{
	if (FocusedInteractive)
		return FocusedInteractive->GetAvailableActions(GetCharacterOwner());

	return TArray<UInteractiveAction*>();
}

UInteractiveAction* UInteractiveManagerComponent::GetPendingInteractiveAction() const
{
	return PendingAction;
}

TArray<FText> UInteractiveManagerComponent::GetInteractiveActions()
{
	if (GetCurrentInteractive())
		return GetCurrentInteractive()->GetActionLabels(GetCharacterOwner());

	return TArray<FText>();
}

void UInteractiveManagerComponent::NextPendingAction()
{
	const TArray<UInteractiveAction*> Actions = GetFocusedInteractiveActions();
	if (Actions.IsEmpty())
		PendingInteractiveIndex = INDEX_NONE;
	else
		PendingInteractiveIndex = (PendingInteractiveIndex + 1) % Actions.Num();


	UpdatePendingActionReference();
}

void UInteractiveManagerComponent::PreviousPendingAction()
{
	const TArray<UInteractiveAction*> Actions = GetFocusedInteractiveActions();

	if (Actions.IsEmpty())	
		PendingInteractiveIndex = INDEX_NONE;			
	else	
		PendingInteractiveIndex = (PendingInteractiveIndex - 1 + Actions.Num()) % Actions.Num();

	UpdatePendingActionReference();
}

void UInteractiveManagerComponent::UpdatePendingActionReference()
{
	PendingAction = nullptr;

	if (!IsValid(FocusedInteractive))
		return;

	const TArray<UInteractiveAction*> Actions = GetFocusedInteractiveActions();

	if (!Actions.IsValidIndex(PendingInteractiveIndex))
		return;

	PendingAction = Actions[PendingInteractiveIndex];
}



//============================================
//===========INTERACTIVES MANAGEMENT==========
//============================================

void UInteractiveManagerComponent::CalcFocusedInteractive()
{
	if (TracedInteractives.IsEmpty())
	{
		FocusedInteractive = nullptr;
		PendingInteractiveIndex = INDEX_NONE;
		PendingAction = nullptr;
		return;
	}

	UInteractiveComponent* InteractiveToFocusOn = TracedInteractives[0];

	for (UInteractiveComponent* Interactive : TracedInteractives)
	{

	}

	if (FocusedInteractive != InteractiveToFocusOn)
	{
		FocusedInteractive = InteractiveToFocusOn;
		PendingInteractiveIndex = 0;

		UpdatePendingActionReference();
	}	
}

void UInteractiveManagerComponent::CalcInteractives()
{
	TArray<UInteractiveComponent*> ToRemove = NearbyInteractives;

	// Add new interactives from traced results
	for (UInteractiveComponent* TracedInteractive : TracedInteractives)
	{
		if (!NearbyInteractives.Contains(TracedInteractive))
		{
			AddInteractive(TracedInteractive);
			NotifyInteractiveOfHover(TracedInteractive);
		}
		// Prevent this interactive from being removed
		ToRemove.Remove(TracedInteractive);
	}

	/*cleanup no-longer valid interactives*/
	// Safely remove interactives that are no longer valid
	ToRemove.RemoveAll([&](UInteractiveComponent* Interactive)
		{
			RemoveInteractive(Interactive);
			NotifyInteractiveOfUnhover(Interactive);
			return true;  // Remove from array
		});
}

bool UInteractiveManagerComponent::IsInteractive(UInteractiveComponent* Interactive)
{
	/*check for valid interfacing implementation*/
	if (!Interactive)
		return false;

	/*check component itself*/
	if (!Interactive->IsInteractionEnabled())
		return false;

	/*return true if all is well*/
	return true;
}

void UInteractiveManagerComponent::AddInteractive(UInteractiveComponent*& Interactive)
{
	NearbyInteractives.AddUnique(Interactive);
}

void UInteractiveManagerComponent::RemoveInteractive(UInteractiveComponent*& Interactive)
{
	NearbyInteractives.Remove(Interactive);
}

void UInteractiveManagerComponent::ClearInteractives()
{
	NearbyInteractives.Empty();
}

void UInteractiveManagerComponent::ClearActiveInteraction()
{
	ActiveInteractive = nullptr;
}

void UInteractiveManagerComponent::UpdateActiveInteractiveStatus()
{
	if (IsActiveInteractionComplete())
		ActiveInteractive = nullptr;
}

void UInteractiveManagerComponent::NotifyInteractiveOfHover(UInteractiveComponent*& Interactive)
{
	if (IsInteractive(Interactive))
		Interactive->NativeOnHovered(GetCharacterOwner());
}

void UInteractiveManagerComponent::NotifyInteractiveOfUnhover(UInteractiveComponent*& Interactive)
{
	if (IsInteractive(Interactive))
		Interactive->NativeOnUnhovered(GetCharacterOwner());
}


UInteractiveComponent* UInteractiveManagerComponent::GetCurrentInteractive() const
{
	if (NearbyInteractives.Num() <= 0)
		return nullptr;

	return NearbyInteractives[0];
}

AActor* UInteractiveManagerComponent::GetCurrentInteractiveOwner()
{
	if (GetCurrentInteractive() == nullptr)
		return nullptr;

	else
		return GetCurrentInteractive()->GetOwner();
}

//===============================
//============TRACING============
//===============================
void UInteractiveManagerComponent::TraceForInteractives()
{
	if (!GetCharacterOwner())
		return;

	/*run only for locally controlled characters to save on performance*/
	if(GetCharacterOwner()->IsLocallyControlled())
		{

			/*clear previous list of traced objects*/
			TracedInteractives.Empty();

			/*initialize properties*/
			TArray<FHitResult> HitResults;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(GetOwner()); //ignore whoever owns us so we dont' hit ourselves

			if (GetWorld()->SweepMultiByChannel(HitResults, GetTraceStart(), GetTraceEnd(), GetTraceRotation().Quaternion(), GetTraceChannel(), FCollisionShape::MakeBox(FVector(ProbeSize)), CollisionParams))
			{
				/*loop through hit results*/
				for (auto HitResult : HitResults)
				{

					if (UInteractiveComponent* InteractiveComponent = GetInteractiveComponentFromHit(HitResult))
					{
						/*if this interactive component is currently not interactable - skip it and move onto the next component*/
						if(!IsInteractive(InteractiveComponent))
							continue;

						TracedInteractives.AddUnique(InteractiveComponent);

						/*debugging*/
						#if WITH_EDITOR
						if (bDebugTrace) 
						{
							DrawDebugSphere(GetWorld(), HitResult.Location, ProbeSize, 12, FColor::Cyan, false, 0.5f);
							UE_LOG(LogTemp, Warning, TEXT("\t Interactive Check: %s"), *GetNameSafe(InteractiveComponent));
						}
							

						#endif

					}
					else
					{
						/*debugging*/
					#if WITH_EDITOR
						if (bDebugTrace)
						{
							UE_LOG(LogTemp, Warning, TEXT("\t Interactive Check: %s"), *GetNameSafe(HitResult.GetComponent()));
							DrawDebugSphere(GetWorld(), HitResult.Location, ProbeSize, 12, FColor::Red, false, 0.5f);
						}
							
					#endif
					}
				}
			}

			/*debugging*/
#if WITH_EDITOR
			/*if (bDebugTrace)
			{
				DrawDebugLine(GetWorld(), GetTraceStart(), GetTraceEnd(), FColor::Blue, false, 0.3f, (uint8)0U, 3.0f);
			}*/
#endif
		}	
}

FVector UInteractiveManagerComponent::GetTraceStart()
{
	/*default location starts at Actor location*/
	FVector StartLocation = GetOwner()->GetActorLocation();
	FRotator Rotation;

	/*look for controller if our owner is a Pawn type*/
	if (TraceType == EInteractiveTraceType::Controller)
	{
		AFirstPersonCharacter* Character = GetCharacterOwner();
		if (Character && Character->GetController())
		{
			Character->GetController()->GetPlayerViewPoint(StartLocation, Rotation);
		}
	}
	/*look for owner actor's camera*/
	else if (TraceType == EInteractiveTraceType::Camera && GetOwnerCamera())
	{
		FMinimalViewInfo ViewInfo;
		GetOwnerCamera()->GetCameraView(UGameplayStatics::GetWorldDeltaSeconds(this), ViewInfo);
		StartLocation = ViewInfo.Location;
	}

	return StartLocation;
}

FVector UInteractiveManagerComponent::GetTraceEnd()
{
	FVector EndLocation = FVector(0);
	FRotator ViewRotation = GetTraceRotation();


	///*look for controller if our owner is a Pawn type*/
	//if (TraceType == EInteractiveTraceType::Controller && Cast<APawn>(GetOwner()))
	//{
	//	APawn* Pawn = Cast<APawn>(GetOwner());
	//	if (Pawn && Pawn->GetController())
	//	{
	//		Pawn->GetController()->GetPlayerViewPoint(EndLocation, ViewRotation);
	//	}
	//}
	///*look for owner actor's camera*/
	//else if (TraceType == EInteractiveTraceType::Camera && GetOwner() && GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()))
	//{

	//}
	///*fallback to actor center otherwise*/
	//else
	//{

	//}

	//return EndLocation + (ViewRotation.Vector() * TraceLength);
	return GetTraceStart() + (ViewRotation.Vector() * TraceLength);
}

FRotator UInteractiveManagerComponent::GetTraceRotation()
{
	FVector Location;
	FRotator TraceRotation = GetOwner()->GetActorRotation();

	/*look for controller if our owner is a Pawn type*/
	if (TraceType == EInteractiveTraceType::Controller && Cast<APawn>(GetOwner()))
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn && Pawn->GetController())
		{
			Pawn->GetController()->GetPlayerViewPoint(Location, TraceRotation);
		}
	}
	/*look for owner actor's camera*/
	else if (TraceType == EInteractiveTraceType::Camera && GetOwnerCamera())
	{
		FMinimalViewInfo ViewInfo;
		GetOwnerCamera()->GetCameraView(UGameplayStatics::GetWorldDeltaSeconds(this), ViewInfo);
		TraceRotation = ViewInfo.Rotation;
	}

	return TraceRotation;
}

ECollisionChannel UInteractiveManagerComponent::GetTraceChannel()
{
	return UEngineTypes::ConvertToCollisionChannel(TraceChannel);
}



bool UInteractiveManagerComponent::IsActiveInteractionComplete()
{
	/*safety check*/
	if (ActiveInteractive == nullptr)
		return true;

	/*initialize & check*/
	IInteractiveShapeInterface* Interactive = Cast<IInteractiveShapeInterface>(ActiveInteractive);
	if (Interactive == nullptr)
		return false;

	if (Interactive->GetRemainingInteractionTime() <= 0)
		return true;
	else
		return false;
}

bool UInteractiveManagerComponent::IsInteracting()
{
	return IsValid(ActiveInteractive) && IsValid(ActiveAction);
}


UInteractiveComponent* UInteractiveManagerComponent::GetInteractiveComponentFromHit(const FHitResult& HitResult) const
{
	USceneComponent* CurrentComp = HitResult.GetComponent();

	/*attempt to return an Interactive Component from the parent hierarchy*/
	while (CurrentComp)
	{
		if(UInteractiveComponent* InteractiveComp = Cast<UInteractiveComponent>(CurrentComp))
			return InteractiveComp;

		CurrentComp = CurrentComp->GetAttachParent();
	}

	return nullptr;
}

AFirstPersonCharacter* UInteractiveManagerComponent::GetCharacterOwner() const
{
	return Cast<AFirstPersonCharacter>(GetOwner());
}

UCameraComponent* UInteractiveManagerComponent::GetOwnerCamera()
{
	if (PlayerCamera != nullptr)
		return PlayerCamera;

	if (GetOwner() == nullptr)
		return nullptr;

	PlayerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	return PlayerCamera;
}