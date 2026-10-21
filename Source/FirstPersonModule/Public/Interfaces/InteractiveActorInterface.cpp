#include "InteractiveActorInterface.h"

void IInteractiveActorInterface::StartInteraction(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{	
	NativeOnInteractionStarted(User, Interactive, Action, Context);
	Execute_BP_OnInteractionStarted(Cast<AActor>(this), User, Interactive, Action, Context);
}

void IInteractiveActorInterface::StopInteraction(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, EInteractionEndReason EndReason, const FInteractionContext& Context)
{
    FInteractionContext TerminalContext = Context;

    TerminalContext.bInterrupted = EndReason == EInteractionEndReason::Interrupted;
	if (EndReason == EInteractionEndReason::Completed)
	{		
		NativeOnInteractionCompleted(User, Interactive, Action, TerminalContext);
		Execute_BP_OnInteractionCompleted(Cast<AActor>(this),User, Interactive, Action, TerminalContext);
	}
		
	else
	{

		NativeOnInteractionCanceled(User,Interactive,Action,TerminalContext);
		Execute_BP_OnInteractionCanceled(Cast<AActor>(this),User,Interactive,Action,TerminalContext);
	}	
}

void IInteractiveActorInterface::NativeOnInteractionStarted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	//override me!
}

void IInteractiveActorInterface::NativeOnInteractionCompleted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	//override me!
}

void IInteractiveActorInterface::NativeOnInteractionCanceled(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, const FInteractionContext& Context)
{
	//override me!
}



