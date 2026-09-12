// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractiveComponent.h"
#include "Interactives/InteractiveActor.h"

/*components*/
#include "Components/InteractiveCollisionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractivePromptComponent.h"

/*characters*/
#include "Characters/FirstPersonCharacter.h"

/*engine*/
#include "Engine/StaticMesh.h"

/*interfaces*/
#include "Interfaces/InteractiveActorInterface.h"


/*materials*/
#include "Materials/MaterialInstanceDynamic.h"

/*logging*/
#include "../Logging.h"

/*replication*/
#include "Net/UnrealNetwork.h"


void UInteractiveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractiveComponent, bIsInteractive);
	DOREPLIFETIME(UInteractiveComponent, bInteractionDisabled);
	DOREPLIFETIME(UInteractiveComponent, bInteractionActive);
	DOREPLIFETIME(UInteractiveComponent, CurrentUser);
}

UInteractiveComponent::UInteractiveComponent()
{
	InteractiveCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interactive Collision"));

	if (InteractiveCollision)
	{
		InteractiveCollision->SetupAttachment(this);
		InteractiveCollision->SetRelativeLocation(FVector::ZeroVector);
		InteractiveCollision->SetRelativeRotation(FRotator::ZeroRotator);

		InteractiveCollision->SetUsingAbsoluteLocation(false);
		InteractiveCollision->SetUsingAbsoluteRotation(false);
		InteractiveCollision->SetUsingAbsoluteScale(false);

		InteractiveCollision->SetCollisionProfileName(TEXT("Interactive"));
		InteractiveCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		InteractiveCollision->InitBoxExtent(FVector(10.0f));
		InteractiveCollision->ShapeColor = FColor(55, 195, 251, 255);
		InteractiveCollision->SetLineThickness(2.0f);		
	}
}

void UInteractiveComponent::OnRegister()
{
	Super::OnRegister();

	if (InteractiveCollision && InteractiveCollision->GetAttachParent() != this)
	{
		InteractiveCollision->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}


#if WITH_EDITOR
void UInteractiveComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();


	///*rebuild collision shape when changed*/
	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, InteractiveShape))
	//{
	//	RebuildCollision();
	//}
	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, BoxExtent) || PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, SphereRadius) || PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, CapsuleRadius) || PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, CapsuleHalfHeight))
	//{
	//	UpdateCollisionProperties();
	//}
}
#endif



// Called when the game starts
void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();

	InitDynamicMaterial();
	/*init collision bindings*/
	//if (InteractiveCollision)
	//{
	//	InteractiveCollision->OnInteractiveHovered.AddDynamic(this,&UInteractiveComponent::NativeOnHovered);
	//}
}


// Called every frame
void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



//void UInteractiveComponent::CallOwnerFunctionByName()
//{
//	
//}

//============================
//============HOVER===========
//============================

void UInteractiveComponent::InitDynamicMaterial()
{
	if(!bHighlightOnHover)
		return;

	UStaticMeshComponent* StaticMesh = GetHoverStaticMesh();

	/*initiate dynamic material*/
	if (DynamicMaterial == nullptr && StaticMesh != nullptr)
	{
		if (UMaterialInterface* DefaultMaterial = StaticMesh->GetMaterial(0))
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial,StaticMesh,FName("InteractiveDynamicMaterial"));
			//DynamicMaterial = CreateDynamicMaterialInstance(0, DefaultMaterial,FName("DynamicButtonMaterial"));
		}
	}
}

UStaticMeshComponent* UInteractiveComponent::GetHoverStaticMesh()
{
	return nullptr;
}

void UInteractiveComponent::HighlightDynamicMaterial()
{
	if(DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(HighlightMaterialParameterName, HighlightMaterialIntensity);
}

void UInteractiveComponent::RemoveHighlightFromDynamicMaterial()
{
	if(DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(HighlightMaterialParameterName, DefaultMaterialIntensity);;
}

void UInteractiveComponent::NativeOnHovered(AFirstPersonCharacter* User)
{
	if (bHovered == false)
	{
		bHovered = true;

		UpdatePromptText(); //get the default action text to show
		TogglePromptVisibility(true); //display prompt

		if (OnInteractiveHovered.IsBound())
			OnInteractiveHovered.Broadcast(User);
	}

	if (bHighlightOnHover)
		HighlightDynamicMaterial();
	
}

void UInteractiveComponent::NativeOnUnhovered(AFirstPersonCharacter* User)
{

	if (bHovered)
	{
		bHovered = false;
		TogglePromptVisibility(false);

		if (OnInteractiveUnhovered.IsBound())
			OnInteractiveUnhovered.Broadcast(User);
	}

	if (bHighlightOnHover)
		RemoveHighlightFromDynamicMaterial();	
}




//============================================
//===================PROMPT===================
//============================================


void UInteractiveComponent::UpdatePromptText()
{
	/*show/hide any prompts attached to us*/
	for (USceneComponent* ChildComponent : GetAttachChildren())
	{
		if (UInteractivePromptComponent* Prompt = Cast<UInteractivePromptComponent>(ChildComponent))
		{
			Prompt->SetPromptText(GetCurrentActionLabel());
		}
	}
}

const TArray<FText> UInteractiveComponent::GetActionLabels(AFirstPersonCharacter* User)
{
	TArray<FText> ActionLabels = TArray<FText>();

	for (uint8 i = 0; i < Interactions.Num(); i++)
	{
		UInteractiveAction* Action = Interactions[i].Action.GetDefaultObject();

		if (Action->CanExecute(GetOwner(), User))
			ActionLabels.Add(Action->InteractionLabel);
	}

	return ActionLabels;
}

const FText UInteractiveComponent::GetCurrentActionLabel()
{
	if (const UInteractiveAction* Action = GetCurrentAction())
	{
		return Action->InteractionLabel;
	}

	return FText();
}

void UInteractiveComponent::TogglePromptVisibility(bool bNewVisibility)
{
	/*show/hide any prompts attached to us*/
	for (USceneComponent* ChildComponent : GetAttachChildren())
	{
		if (UInteractivePromptComponent* Prompt = Cast<UInteractivePromptComponent>(ChildComponent))
		{
			Prompt->SetVisibility(bNewVisibility);
		}
	}
}

//=========================================
//=================ACTIONS=================
//=========================================

const UInteractiveAction* UInteractiveComponent::GetCurrentAction() const
{
	if (!Interactions.IsValidIndex(InteractionIndex))
		return nullptr;

	return Interactions[InteractionIndex].Action.GetDefaultObject();
}

const FName UInteractiveComponent::GetCurrentActionFunctionToCall() const
{
	if (!Interactions.IsValidIndex(InteractionIndex))
		return "N/A";

	return Interactions[InteractionIndex].FunctionToCall;
}

//=================================================
//===================INTERACTION===================
//=================================================


bool UInteractiveComponent::IsInteractionAvailable(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	if (bInteractionDisabled)
		return false;

	/*if our owner (actor) is using the InteractiveActorInterface let them override the results*/
	//if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
	//	return Cast<IInteractiveActorInterface>(GetOwner())->Execute_IsInteractionAvailable(GetOwner(), User, this, Action);

	return bIsInteractive;
}

/*whether or not this interactive should be pickedu up by traces currently or not*/
bool UInteractiveComponent::IsInteractionEnabled()
{
	return !bInteractionDisabled;
}


//void UInteractiveComponent::Interact(AFirstPersonCharacter* User)
//{
//	/*if (bAutoCallFunction)
//		CallOwnerFunctionByName();*/
//
//	NativeOnInteraction(User);
//
//	/*blueprint version*/
//	BP_OnInteraction(User);
//
//	/*if (OnInteraction.IsBound())
//		OnInteraction.Broadcast(User);*/
//}

/*starts the interaction*/
bool UInteractiveComponent::BeginInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	//const UInteractiveAction* Action = GetCurrentAction();
	const FName FunctionName = GetCurrentActionFunctionToCall();

	/*deny interaction*/
	if (!(IsInteractionAvailable)(User, Action))
	{
		DenyInteraction(User);
		return false;
	}

	SetActiveUser(User);

	/*notify anyone listening to our component*/
	if (OnInteractionBegin.IsBound())
		OnInteractionBegin.Broadcast(User, Action);

	/*if a custom function is declared in the Blueprint - try to call that function on our owner*/
	if (!FunctionName.IsNone())
		CallOwnerFunctionByName(FunctionName, User, Action);


	return true;
}

void UInteractiveComponent::StopInteraction(AFirstPersonCharacter* User)
{

}


void UInteractiveComponent::CancelInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	RemoveActiveUser(User);

	/*if our owner (actor) is using the InteractiveActorInterface notify them of the interaction*/
	//if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
	//	Cast<IInteractiveActorInterface>(GetOwner())->Execute_OnInteractionCanceled(GetOwner(), User, this, Action);

	/*Interaction Canceled*/
	//if (OnInteractionCanceled.IsBound())
		//OnInteractionCanceled.Broadcast(GetOwner(), User);
}

void UInteractiveComponent::CompleteInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	RemoveActiveUser(User);

	/*if our owner (actor) is using the InteractiveActorInterface notify them of the interaction*/
	//if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
	//	Cast<IInteractiveActorInterface>(GetOwner())->Execute_OnInteractionCompleted(GetOwner(), User, this, Action);

	/*interaction completed*/
	//if (OnInteractionCompleted.IsBound())
		//OnInteractionCompleted.Broadcast(GetOwner(), User);
}

void UInteractiveComponent::DenyInteraction(AFirstPersonCharacter* User)
{
	User->GetInteractionsManager()->ReceiveDeniedInteraction(GetOwner());

	//if (OnInteractionDenied.IsBound())
	//	OnInteractionCompleted.Broadcast(GetOwner(), User);
}


float UInteractiveComponent::GetCurrentInteractionTime()
{
	return 0.0f;
}

float UInteractiveComponent::GetRemainingInteractionTime()
{
	return 0.0f;
}


	//===============================================================
	//===========================COLLISION===========================
	//===============================================================

//void UInteractiveComponent::RebuildCollision()
//{
//	if (!GetOwner())
//		return;
//
//	if (InteractiveCollision)
//	{
//		InteractiveCollision->DestroyComponent();
//		InteractiveCollision = nullptr;
//	}
//
//	switch (InteractiveShape)
//	{
//	case EInteractiveShape::Box:
//		{
//			UBoxComponent* Shape = NewObject<UBoxComponent>(GetOwner(), NAME_None, RF_Transactional);
//			Shape->InitBoxExtent(BoxExtent);
//			InteractiveCollision = Shape;
//			break;
//		}
//
//	case EInteractiveShape::Sphere:
//		{
//			USphereComponent* Shape = NewObject<USphereComponent>(GetOwner(), NAME_None, RF_Transactional);
//			Shape->InitSphereRadius(SphereRadius);
//			InteractiveCollision = Shape;
//			break;
//		}
//
//	case EInteractiveShape::Capsule:
//		{
//			UCapsuleComponent* Shape = NewObject<UCapsuleComponent>(GetOwner(), NAME_None, RF_Transactional);
//			Shape->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
//			InteractiveCollision = Shape;
//			break;
//		}
//	}
//
//	if (!InteractiveCollision)
//		return;
//
//	GetOwner()->AddInstanceComponent(InteractiveCollision);
//
//	InteractiveCollision->SetupAttachment(this);
//	InteractiveCollision->SetRelativeTransform(FTransform::Identity);
//	InteractiveCollision->SetCollisionProfileName(TEXT("Interactive"));
//	InteractiveCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
//	InteractiveCollision->RegisterComponent();
//}
//
//void UInteractiveComponent::UpdateCollisionProperties()
//{
//	if (UBoxComponent* Box = Cast<UBoxComponent>(InteractiveCollision))
//	{
//		Box->SetBoxExtent(BoxExtent);
//	}
//	else if (USphereComponent* Sphere = Cast<USphereComponent>(InteractiveCollision))
//	{
//		Sphere->SetSphereRadius(SphereRadius);
//	}
//	else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(InteractiveCollision))
//	{
//		Capsule->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
//	}
//}




//=============================================================
//==========================UTILITIES==========================
//=============================================================

void UInteractiveComponent::CallOwnerFunctionByName(const FName FunctionName, const AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	if (FunctionName.IsNone())
	{
		UE_LOG(LogFirstPersonModule, Warning, TEXT("%s::CallOwnerFunctionByName() - no function name set"), *GetName());
		return;
	}

	if (!GetOwner())
	{
		UE_LOG(LogFirstPersonModule, Warning, TEXT("%s::CallOwnerFunctionByName() - component has no owner!"), *GetName());
		return;
	}

	UFunction* Function = GetOwner()->FindFunction(FunctionName);
	if (!IsValid(Function))
	{
		UE_LOG(LogFirstPersonModule, Warning, TEXT("!!!%s::CallOwnerFunctionByName() - Function (%s) not found in owner (%s)"), *GetName(), *FunctionName.ToString(), *GetNameSafe(GetOwner()));
		return;
	}

	FInteractionEventParams Params;
	Params.Action = Action;
	Params.User = User;
	//Params.CollisionComponent = this;

	GetOwner()->ProcessEvent(Function, &Params);
}

void UInteractiveComponent::SetActiveUser(AFirstPersonCharacter* User)
{
	/*beginning of interaction starts*/
	bInteractionActive = true;
	CurrentUser = User;
}

void UInteractiveComponent::RemoveActiveUser(AFirstPersonCharacter* User)
{
	if (CurrentUser == User)
	{
		bInteractionActive = false;
		CurrentUser = nullptr;
	}
}