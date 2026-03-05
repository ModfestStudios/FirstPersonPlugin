// Copyright (c) 2022 Pocket Sized Animations


#include "Components/InteractiveCollisionComponent.h"
#include "Components/InteractivePromptComponent.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "Characters/FirstPersonCharacter.h"

/*logging*/
#include "../Logging.h"

/*replication*/
#include "Net/UnrealNetwork.h"

void UInteractiveCollisionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractiveCollisionComponent, bIsInteractive);
	DOREPLIFETIME(UInteractiveCollisionComponent, bInteractionDisabled);
	DOREPLIFETIME(UInteractiveCollisionComponent, bInteractionActive);
	DOREPLIFETIME(UInteractiveCollisionComponent, CurrentUser);
}

UInteractiveCollisionComponent::UInteractiveCollisionComponent()
{
	SetCollisionProfileName("Interactive");
	CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	/*Editor Visualizer*/
	ShapeColor = FColor(55, 195, 251, 255);
	LineThickness = 1.0f;

	/*repliclation*/
	SetIsReplicatedByDefault(true);
}

void UInteractiveCollisionComponent::UpdatePromptText()
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

const TArray<FText> UInteractiveCollisionComponent::GetActionLabels(AFirstPersonCharacter* User)
{
	TArray<FText> ActionLabels = TArray<FText>();

	for (uint8 i = 0; i < Interactions.Num(); i++)
	{
		UInteractiveAction* Action = Interactions[i].Action.GetDefaultObject();

		if (Action->CanExecute(GetOwner(),User))
			ActionLabels.Add(Action->InteractionLabel);
	}

	return ActionLabels;
}

const FText UInteractiveCollisionComponent::GetCurrentActionLabel()
{
	if (const UInteractiveAction* Action = GetCurrentAction())
	{
		return Action->InteractionLabel;
	}

	return FText();
}

const UInteractiveAction* UInteractiveCollisionComponent::GetCurrentAction() const
{
	if (!Interactions.IsValidIndex(InteractionIndex))
		return nullptr;

	return Interactions[InteractionIndex].Action.GetDefaultObject();
}

const FName UInteractiveCollisionComponent::GetCurrentActionFunctionToCall() const
{
	if (!Interactions.IsValidIndex(InteractionIndex))
		return "N/A";

	return Interactions[InteractionIndex].FunctionToCall;
}

bool UInteractiveCollisionComponent::IsInteractionAvailable(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	if (bInteractionDisabled)
		return false;

	/*if our owner (actor) is using the InteractiveActorInterface let them override the results*/
	if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
		return Cast<IInteractiveActorInterface>(GetOwner())->Execute_IsInteractionAvailable(GetOwner(), User, this, Action);

	return bIsInteractive;
}

/*whether or not this interactive should be pickedu up by traces currently or not*/
bool UInteractiveCollisionComponent::IsInteractionEnabled()
{
	return !bInteractionDisabled;
}

/*starts the interaction*/
bool UInteractiveCollisionComponent::BeginInteraction(AFirstPersonCharacter* User)
{	
	const UInteractiveAction* Action = GetCurrentAction();
	const FName FunctionName = GetCurrentActionFunctionToCall();

	/*deny interaction*/
	if (!(IsInteractionAvailable)(User, Action))
	{
		DenyInteraction(User);
		return false;
	}

	SetActiveUser(User);

	/*notify anyone listening to our component*/
	if (OnInteractionStarted.IsBound())
		OnInteractionStarted.Broadcast(User, Action);

	/*if our owner (actor) is using the InteractiveActorInterface notify them of the interaction*/
	if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
		Cast<IInteractiveActorInterface>(GetOwner())->OnInteraction(User, this, Action);


	/*if a custom function is declared in the Blueprint - try to call that function on our owner*/
	if(!FunctionName.IsNone())
		CallOwnerFunctionByName(FunctionName, User, Action);
	
	/*notify*/
	//if (OnInteractionBegin.IsBound())
		//OnInteractionBegin.Broadcast(GetOwner(), User);

	return true;
}

void UInteractiveCollisionComponent::StopInteraction(AFirstPersonCharacter* User)
{

}


void UInteractiveCollisionComponent::CancelInteraction(AFirstPersonCharacter* User,const UInteractiveAction* Action)
{
	RemoveActiveUser(User);

	/*if our owner (actor) is using the InteractiveActorInterface notify them of the interaction*/
	if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
		Cast<IInteractiveActorInterface>(GetOwner())->Execute_OnInteractionCanceled(GetOwner(),User, this, Action);

	/*Interaction Canceled*/
	//if (OnInteractionCanceled.IsBound())
		//OnInteractionCanceled.Broadcast(GetOwner(), User);
}

void UInteractiveCollisionComponent::CompleteInteraction(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	RemoveActiveUser(User);

	/*if our owner (actor) is using the InteractiveActorInterface notify them of the interaction*/
	if (GetOwner()->GetClass()->ImplementsInterface(UInteractiveActorInterface::StaticClass()))
		Cast<IInteractiveActorInterface>(GetOwner())->Execute_OnInteractionCompleted(GetOwner(),User, this, Action);

	/*interaction completed*/
	//if (OnInteractionCompleted.IsBound())
		//OnInteractionCompleted.Broadcast(GetOwner(), User);
}

void UInteractiveCollisionComponent::DenyInteraction(AFirstPersonCharacter* User)
{	
	User->GetInteractionsManager()->ReceiveDeniedInteraction(GetOwner());	

	//if (OnInteractionDenied.IsBound())
	//	OnInteractionCompleted.Broadcast(GetOwner(), User);
}


void UInteractiveCollisionComponent::OnHoverBegin(AFirstPersonCharacter* User)
{
	if (bHovered == false)
	{
		bHovered = true;

		UpdatePromptText(); //get the default action text to show
		TogglePromptVisibility(true); //display prompt

		if (OnInteractiveHovered.IsBound())
			OnInteractiveHovered.Broadcast(User);
	}
}

void UInteractiveCollisionComponent::OnHoverEnd(AFirstPersonCharacter* User)
{
	if (bHovered)
	{
		bHovered = false;
		TogglePromptVisibility(false);

		if (OnInteractiveUnhovered.IsBound())
			OnInteractiveUnhovered.Broadcast(User);
	}
}

void UInteractiveCollisionComponent::TogglePromptVisibility(bool bNewVisibility)
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

float UInteractiveCollisionComponent::GetCurrentInteractionTime()
{
	return 0.0f;
}

float UInteractiveCollisionComponent::GetRemainingInteractionTime()
{
	return 0.0f;
}

void UInteractiveCollisionComponent::CallOwnerFunctionByName(const FName FunctionName, const AFirstPersonCharacter* User, const UInteractiveAction* Action)
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
	Params.CollisionComponent = this;

	GetOwner()->ProcessEvent(Function, &Params);
}

void UInteractiveCollisionComponent::SetActiveUser(AFirstPersonCharacter* User)
{
	/*beginning of interaction starts*/
	bInteractionActive = true;
	CurrentUser = User;
}

void UInteractiveCollisionComponent::RemoveActiveUser(AFirstPersonCharacter* User)
{
	if (CurrentUser == User)
	{
		bInteractionActive = false;
		CurrentUser = nullptr;
	}
}
