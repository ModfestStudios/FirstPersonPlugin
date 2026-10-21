// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractiveComponent.h"
#include "Interactives/InteractiveActor.h"
#include "Interactives/InteractiveAction.h"
#include "Interactives/Actions/PickupAction.h"

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
#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

/*interfaces*/
#include "Interfaces/InteractiveActorInterface.h"


/*materials*/
#include "Materials/MaterialInstanceDynamic.h"

/*logging*/
#include "../Logging.h"

/*subsystems*/
#include "Subsystems/InteractiveSubsystem.h"

/*replication*/
#include "Net/UnrealNetwork.h"

void UInteractiveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractiveComponent, bIsInteractive);
	DOREPLIFETIME(UInteractiveComponent, bInteractionDisabled);
	DOREPLIFETIME(UInteractiveComponent, bInteractionActive);
	DOREPLIFETIME(UInteractiveComponent, ActiveUser);
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
		InteractiveCollision->InitBoxExtent(BoxExtent);
		InteractiveCollision->ShapeColor = FColor(55, 195, 251, 255);
		InteractiveCollision->SetLineThickness(CollisionLineThickness);
	}

	bAutoActivate = true;
}

void UInteractiveComponent::OnRegister()
{
	Super::OnRegister();

	if (InteractiveCollision && InteractiveCollision->GetAttachParent() != this)
	{
		InteractiveCollision->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	UpdateCollisionProperties();
}

#if WITH_EDITOR
void UInteractiveComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();


	const FName MemberPropertyName = PropertyChangedEvent.MemberProperty
		? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	// MemberProperty identifies BoxExtent when an individual X/Y/Z field is edited.
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, BoxExtent) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, BoxExtent) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, CollisionLineThickness))
	{
		UpdateCollisionProperties();
	}

	// Super may reconstruct Blueprint components; update the current instance first.
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Called when the game starts
void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeActions();

	/*register component*/
	RegisterInteractive();

	/*create dynamic material if needed*/
	InitDynamicMaterial();

	/*init collision bindings*/
	//if (InteractiveCollision)
	//{
	//	InteractiveCollision->OnInteractiveHovered.AddDynamic(this,&UInteractiveComponent::NativeOnHovered);
	//}
}

void UInteractiveComponent::InitializeActions()
{
	for (const FInteractiveActionConfig& Config : ActionConfigs)
	{
		if(!Config.Action)
			continue;

		UInteractiveAction* Action = NewObject<UInteractiveAction>(this, Config.Action);

		if(!Action)
			continue;

		Action->Initialize(this, Config);

		RuntimeActions.Add(Action);
	}
}

void UInteractiveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInteractive();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UInteractiveAction* Action : RuntimeActions)
	{
		if(!IsValid(Action))
			continue;

		if (IsActionInProgress(Action))
		{
			if (UInteractiveManagerComponent* InteractionManager = ActiveUser->GetComponentByClass<UInteractiveManagerComponent>())
			{
				if (ActiveInteractionDuration > 0.0f)
				{
					float StartTimeStamp = InteractionManager->InteractionStartTime;
					float CurrentTimeStamp = GetWorld()->GetTimeSeconds();
					float ElapsedTime = CurrentTimeStamp - StartTimeStamp;

					if(ElapsedTime >= ActiveInteractionDuration)
						NativeOnStopInteraction(ActiveUser,Action,EInteractionEndReason::Completed,GetInteractionContext());
				}

			}
		}
	}
}

void UInteractiveComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

//================================================
//=====================HOVER======================
//================================================

//void UInteractiveComponent::CallOwnerFunctionByName()
//{
//
//}
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

//================================================
//=====================PROMPT=====================
//================================================

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

	//for (uint8 i = 0; i < ActionConfigs.Num(); i++)
	//{
	//	UInteractiveAction* Action = ActionConfigs[i].Action.GetDefaultObject();

	//	if (Action->CanExecute(GetOwner(), User))
	//		ActionLabels.Add(Action->InteractionLabel);
	//}

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

//================================================
//====================ACTIONS=====================
//================================================

const UInteractiveAction* UInteractiveComponent::GetCurrentAction() const
{
	if (!ActionConfigs.IsValidIndex(InteractionIndex))
		return nullptr;

	return ActionConfigs[InteractionIndex].Action.GetDefaultObject();
}

UInteractiveAction* UInteractiveComponent::GetActionByID(FGuid ID) const
{
	for (UInteractiveAction* Action : RuntimeActions)
	{
		if(Action->ID == ID)
			return Action;
	}

	return nullptr;
}

const FName UInteractiveComponent::GetCurrentActionFunctionToCall() const
{
	if (!ActionConfigs.IsValidIndex(InteractionIndex))
		return "N/A";

	return ActionConfigs[InteractionIndex].FunctionToCall;
}

TArray<UInteractiveAction*> UInteractiveComponent::GetAvailableActions(AFirstPersonCharacter* User) const
{
	TArray<UInteractiveAction*> AvailableActions;

	for (const UInteractiveAction* Action : RuntimeActions)
	{
		//UE_LOG(LogTemp,Log,TEXT("===============GetAvailableActions()==============="));
		if(IsActionAvailable(User, Action))
			AvailableActions.Add(const_cast<UInteractiveAction*>(Action));
	}

	return AvailableActions;
}

bool UInteractiveComponent::IsActionAvailable(AFirstPersonCharacter* User, const UInteractiveAction* Action) const
{
	//UE_LOG(LogTemp,Warning,TEXT("%s::IsActionAvailable(%s,%s)"),*GetName(),*GetNameSafe(User),*GetNameSafe(Action));
	if (!User || !Action)
		return false;

	AActor* Owner = GetOwner();

	if (!Owner)
	{
		//UE_LOG(LogTemp, Warning, TEXT("\t NO OWNER FOUND"));
		return false;
	}


	/*let the owning Actor make the final contextual decision*/
	if (IInteractiveActorInterface* InteractiveInterface = Cast<IInteractiveActorInterface>(Owner))
	{
		bool bActorResult = InteractiveInterface->Execute_IsActionAvailable(Owner,User,this,Action);

		//UE_LOG(LogTemp,Log, TEXT("\t %s Result : %d"),*GetNameSafe(Owner),bActorResult);

		return bActorResult;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("\t NOT AN INTERACTIVE ACTOR INTERFACE"));
	}

	/*otherwise we return the action's default state*/
	bool bActionResult = Action->IsActionAvailable(this, Owner, User);

	return bActionResult;
}

//================================================
//==================INTERACTION===================
//================================================

FInteractionContext UInteractiveComponent::GetInteractionContext() const
{
    if (IsValid(ActiveUser))
        if (UInteractiveManagerComponent* Manager = ActiveUser->GetInteractionsManager())
            return Manager->GetInteractionContext(this, ActiveAction);
    return FInteractionContext();
}

/*whether or not this interactive should be pickedu up by traces currently or not*/
bool UInteractiveComponent::IsInteractionEnabled() const
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
bool UInteractiveComponent::IsActionInProgress(const UInteractiveAction* Action) const
{
	return bInteractionActive &&
			IsValid(ActiveUser) &&
			IsValid(ActiveAction) &&
			ActiveAction == Action;
}

/*starts the interaction*/
bool UInteractiveComponent::NativeOnBeginInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	if (!IsValid(User) || !IsValid(Action) || !Action->bEnabled || bInteractionActive)
        return false;

    FInteractionContext StartedContext = Context;

    StartedContext.bInterrupted = false;
    ActiveInteractionDuration = Action->InteractionType == EInteractionType::Manual ? 0.0f
        : ((StartedContext.bLongHeld && Action->IsA<UPickupAction>()) ? Action->LongHeldActionDuration
           : (Action->InteractionType == EInteractionType::Duration ? Action->ActionDuration : 0.0f));

    /*deny interaction if unavailable*/
	if (!(IsActionAvailable)(User, Action))
	{
		NativeOnDenyInteraction(User, Action);
		return false;
	}

	bInteractionActive = true;

	/*mark our user*/
	SetActiveUser(User);
	ActiveAction = const_cast<UInteractiveAction*>(Action);

	/*notify the interactive manager that the action has been accepted and officially started*/
	User->GetInteractionsManager()->NativeOnInteractionStarted(this, Action, StartedContext);

	/*notify the action*/
	Action->NativeOnInteractionStarted(this, GetOwner(), User, StartedContext);

	if(Action->FunctionType == EInteractionFunctionType::NativeEvent)
	{
		if(IInteractiveActorInterface* ActorInterface = Cast<IInteractiveActorInterface>(GetOwner()))
			ActorInterface->StartInteraction(User,this,Action,StartedContext);


	/*notify anyone listening to our component*/
		if (OnInteractionBegin.IsBound())
			OnInteractionBegin.Broadcast(User, Action, StartedContext);
	}

	else if (Action->FunctionType == EInteractionFunctionType::CustomFunction)
	{
		if(Action->FunctionToCall.IsNone())
		{
			UE_LOG(LogTemp,Warning,TEXT("%s::NativeOnBeginInteraction() - %s (Action) DID NOT DEFINE FUNCTION NAME FOR CUSTOM FUNCTION CALL"),*GetName(), *GetNameSafe(Action));
			return false;
		}

		const FName FunctionName = Action->FunctionToCall;

		CallOwnerFunctionByName(FunctionName, User, Action, StartedContext);
	}

	/*go ahead and just instantly notify we've completed*/
	if(Action->InteractionType != EInteractionType::Manual && ActiveInteractionDuration <= 0.0f)
		NativeOnStopInteraction(User,Action,EInteractionEndReason::Completed,StartedContext);


	return true;
}

void UInteractiveComponent::NativeOnStopInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action, EInteractionEndReason EndReason, const FInteractionContext& Context)
{
    if (!IsValid(User) || ActiveUser != User || !IsActionInProgress(Action))
        return;
	if (IsActionInProgress(Action))
	{
		FInteractionContext TerminalContext = Context;

        TerminalContext.bLongHeld = GetInteractionContext().bLongHeld;
        TerminalContext.bInterrupted = EndReason == EInteractionEndReason::Interrupted;
		RemoveActiveUser(User);

		/*notify the interactive manager that we've been succesful*/
		if(EndReason == EInteractionEndReason::Completed)
			User->GetInteractionsManager()->NativeOnInteractionCompleted(this, Action, TerminalContext);
		else
			User->GetInteractionsManager()->NativeOnInteractionCancelled(this, Action, TerminalContext);

		if (Action->FunctionType == EInteractionFunctionType::NativeEvent)
		{
			if (IInteractiveActorInterface* ActorInterface = Cast<IInteractiveActorInterface>(GetOwner()))
				ActorInterface->StopInteraction(User, this, Action, EndReason, TerminalContext);


		/*notify anyone listening to our component*/
			if (OnInteractionEnd.IsBound())
				OnInteractionEnd.Broadcast(User, Action,EndReason,TerminalContext);
		}

		if(EndReason == EInteractionEndReason::Completed)
			Action->NativeOnInteractionCompleted(this,GetOwner(),User,TerminalContext);
		else
			Action->NativeOnInteractionCanceled(this,GetOwner(),User,TerminalContext);
	}

	bInteractionActive = false;
	ActiveAction = nullptr;
}

void UInteractiveComponent::CancelInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
    NativeOnStopInteraction(User, Action, EInteractionEndReason::Canceled, GetInteractionContext());
}

void UInteractiveComponent::CompleteInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
    NativeOnStopInteraction(User, Action, EInteractionEndReason::Completed, GetInteractionContext());
}

void UInteractiveComponent::NativeOnDenyInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{


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

//================================================
//===================COLLISION====================
//================================================

void UInteractiveComponent::UpdateCollisionProperties()
{
	BoxExtent = BoxExtent.ComponentMax(FVector::ZeroVector);
	CollisionLineThickness = FMath::Max(0.0f, CollisionLineThickness);
	if (InteractiveCollision)
	{
		InteractiveCollision->SetBoxExtent(BoxExtent);
		InteractiveCollision->SetLineThickness(CollisionLineThickness);
	}
}

//================================================
//==================REGISTRATION==================
//================================================

void UInteractiveComponent::RegisterInteractive()
{
	if (UInteractiveSubsystem* ISS = GetWorld()->GetSubsystem<UInteractiveSubsystem>())
	{
		ISS->RegisterInteractiveComponent(this);
	}
}

void UInteractiveComponent::UnregisterInteractive()
{
	if (UInteractiveSubsystem* ISS = GetWorld()->GetSubsystem<UInteractiveSubsystem>())
	{
		ISS->UnregisterInteractiveComponent(this);
	}
}

//================================================
//======================GRID======================
//================================================

FIntPoint UInteractiveComponent::GetGridKey() const
{
	return RegisteredGridKey;
}

void UInteractiveComponent::UpdateGridKey(FIntPoint NewGridKey)
{
	RegisteredGridKey = NewGridKey;
}

//================================================
//===================UTILITIES====================
//================================================

void UInteractiveComponent::CallOwnerFunctionByName(const FName FunctionName, const AFirstPersonCharacter* User, const UInteractiveAction* Action, const FInteractionContext& Context)
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
    Params.Context = Context;
	Params.User = User;
	//Params.CollisionComponent = this;

	GetOwner()->ProcessEvent(Function, &Params);
}

void UInteractiveComponent::SetActiveUser(AFirstPersonCharacter* User)
{
	/*beginning of interaction starts*/
	bInteractionActive = true;
	ActiveUser = User;
}

void UInteractiveComponent::RemoveActiveUser(AFirstPersonCharacter* User)
{
	if (ActiveUser == User)
	{
		bInteractionActive = false;
		ActiveUser = nullptr;
	}
}
