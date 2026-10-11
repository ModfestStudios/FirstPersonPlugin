#include "InteractiveActorInterface.h"

void IInteractiveActorInterface::StartInteraction(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action)
{	
	NativeOnInteractionStarted(User, Interactive, Action);
	Execute_BP_OnInteractionStarted(Cast<AActor>(this), User, Interactive, Action);	
}

void IInteractiveActorInterface::StopInteraction(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, EInteractionEndReason EndReason)
{
	if (EndReason == EInteractionEndReason::Completed)
	{		
		NativeOnInteractionCompleted(User, Interactive, Action);
		Execute_BP_OnInteractionCompleted(Cast<AActor>(this),User, Interactive, Action);
	}
		
	else
	{
		const bool bInterrupted = EndReason == EInteractionEndReason::Interrupted;
		NativeOnInteractionCanceled(User,Interactive,Action,bInterrupted);
		Execute_BP_OnInteractionCanceled(Cast<AActor>(this),User,Interactive,Action,bInterrupted);
	}	
}

void IInteractiveActorInterface::NativeOnInteractionStarted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action)
{
	//override me!
}

void IInteractiveActorInterface::NativeOnInteractionCompleted(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action)
{
	//override me!
}

void IInteractiveActorInterface::NativeOnInteractionCanceled(AFirstPersonCharacter* User, UInteractiveComponent* Interactive, const UInteractiveAction* Action, bool bInterrupted)
{
	//override me!
}



