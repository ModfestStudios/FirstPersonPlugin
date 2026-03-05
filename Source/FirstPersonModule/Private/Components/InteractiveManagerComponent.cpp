// Copyright (c) 2022 Pocket Sized Animations


#include "Components/InteractiveManagerComponent.h"
#include "Components/InteractiveCollisionComponent.h"
#include "Camera/CameraComponent.h"
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

	/*perform trace*/
	TraceForInteractives();
	CalcInteractives();
	UpdateActiveInteractiveStatus();
}

//================================================
//==================INTERACTIONS==================
//================================================

void UInteractiveManagerComponent::Interact()
{
	if (IsValid(GetCurrentInteractive()))
		BeginInteraction(GetCurrentInteractive());
}

void UInteractiveManagerComponent::StopInteraction()
{
	if (IsInteracting())
		EndInteraction(ActiveInteraction);
}

const TArray<FText> UInteractiveManagerComponent::GetInteractiveActions()
{	
	if (GetCurrentInteractive())	
		return GetCurrentInteractive()->GetActionLabels(GetCharacterOwner());

	return TArray<FText>();
}


bool UInteractiveManagerComponent::BeginInteraction(UInteractiveCollisionComponent* Interactive)
{
	/*safety check*/
	if (!Interactive)
		return false;

	/*initialize*/
	bool bInteractionSuccessful = false;
	/*attempt interaction*/
	if (IsInteractive(Interactive))
	{
		/*clients need to request to interact*/
		if (GetNetMode() == NM_Client)
		{
			ServerRequestInteract(Interactive);			
			//return true;
		}

		bInteractionSuccessful = Interactive->BeginInteraction(GetCharacterOwner());
	}

	/*broadcast results*/
	if (bInteractionSuccessful) {

		ActiveInteraction = Interactive;
		return true;
	}
	else
	{
		/*broadcast failure*/
		if (OnInteractionDenied.IsBound())
			OnInteractionDenied.Broadcast(GetCharacterOwner());

		return false;
	}
}

bool UInteractiveManagerComponent::ServerRequestInteract_Validate(UInteractiveCollisionComponent* Interactive)
{
	return true;
}

/*ServerRequestInteract() - client requesting the server to let them begin interacting with this item*/
void UInteractiveManagerComponent::ServerRequestInteract_Implementation(UInteractiveCollisionComponent* Interactive)
{
	BeginInteraction(Interactive);
}

void UInteractiveManagerComponent::EndInteraction(UInteractiveCollisionComponent* Interactive)
{
	if (!Interactive)
		return;

	Interactive->StopInteraction(GetCharacterOwner());
}




//============================================
//===========INTERACTIVES MANAGEMENT==========
//============================================

void UInteractiveManagerComponent::CalcInteractives()
{
	TArray<UInteractiveCollisionComponent*> ToRemove = Interactives;

	// Add new interactives from traced results
	for (UInteractiveCollisionComponent* TracedInteractive : TracedInteractives)
	{
		if (!Interactives.Contains(TracedInteractive))
		{
			AddInteractive(TracedInteractive);
			NotifyInteractiveOfHover(TracedInteractive);
		}
		// Prevent this interactive from being removed
		ToRemove.Remove(TracedInteractive);
	}

	/*cleanup no-longer valid interactives*/
	// Safely remove interactives that are no longer valid
	ToRemove.RemoveAll([&](UInteractiveCollisionComponent* Interactive)
		{
			RemoveInteractive(Interactive);
			NotifyInteractiveOfUnhover(Interactive);
			return true;  // Remove from array
		});
}

bool UInteractiveManagerComponent::IsInteractive(UInteractiveCollisionComponent* Interactive)
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

void UInteractiveManagerComponent::AddInteractive(UInteractiveCollisionComponent*& Interactive)
{
	Interactives.AddUnique(Interactive);
}

void UInteractiveManagerComponent::RemoveInteractive(UInteractiveCollisionComponent*& Interactive)
{
	Interactives.Remove(Interactive);
}

void UInteractiveManagerComponent::ClearInteractives()
{
	Interactives.Empty();
}

void UInteractiveManagerComponent::ClearActiveInteraction()
{
	ActiveInteraction = nullptr;
}

void UInteractiveManagerComponent::UpdateActiveInteractiveStatus()
{
	if (IsActiveInteractionComplete())
		ActiveInteraction = nullptr;
}

void UInteractiveManagerComponent::NotifyInteractiveOfHover(UInteractiveCollisionComponent*& Interactive)
{
	if (IsInteractive(Interactive))
		Interactive->OnHoverBegin(GetCharacterOwner());
}

void UInteractiveManagerComponent::NotifyInteractiveOfUnhover(UInteractiveCollisionComponent*& Interactive)
{
	if (IsInteractive(Interactive))
		Interactive->OnHoverEnd(GetCharacterOwner());
}

void UInteractiveManagerComponent::ReceiveDeniedInteraction(AActor* DeniedActor)
{
	//if (OnInteractionDenied.IsBound())
	//	OnInteractionDenied.Broadcast(DeniedActor, GetOwner());
}

UInteractiveCollisionComponent* UInteractiveManagerComponent::GetCurrentInteractive()
{
	if (Interactives.Num() <= 0)
		return nullptr;

	return Interactives[0];
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
					if (UInteractiveCollisionComponent* HitComp = Cast< UInteractiveCollisionComponent>(HitResult.GetComponent()))
					{
						/*if this interactive component is currently not interactable - skip it and move onto the next component*/
						if(!IsInteractive(HitComp))
							continue;

						TracedInteractives.AddUnique(HitComp);

						/*debugging*/
#if WITH_EDITOR
						if (bDebugTrace)
							DrawDebugSphere(GetWorld(), HitResult.Location, ProbeSize, 12, FColor::Cyan, false, 0.5f);
#endif

					}
					else
					{
						/*debugging*/
#if WITH_EDITOR
						if (bDebugTrace)
							DrawDebugSphere(GetWorld(), HitResult.Location, ProbeSize, 12, FColor::Red, false, 0.5f);
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
	if (ActiveInteraction == nullptr)
		return true;

	/*initialize & check*/
	IInteractiveShapeInterface* Interactive = Cast<IInteractiveShapeInterface>(ActiveInteraction);
	if (Interactive == nullptr)
		return false;

	if (Interactive->GetRemainingInteractionTime() <= 0)
		return true;
	else
		return false;
}

bool UInteractiveManagerComponent::IsInteracting()
{
	if (IsValid(ActiveInteraction))
		return true;

	return false;
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