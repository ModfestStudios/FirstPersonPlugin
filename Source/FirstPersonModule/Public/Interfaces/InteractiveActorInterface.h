// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactives/Interactives.h"
#include "Components/InteractiveCollisionComponent.h"
#include "Interactives/InteractiveAction.h"
#include "InteractiveActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIRSTPERSONMODULE_API IInteractiveActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*called by the InteractiveComponent whenever someone tries to use this - don't override these use the NativeOn events instead*/
	
	void StartInteraction(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
	void StopInteraction(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, EInteractionEndReason EndReason, const FInteractionContext& Context);
	

protected:
	/*override these in your actor*/
	virtual void NativeOnInteractionStarted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
	virtual void NativeOnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
	virtual void NativeOnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);


	/*blueprint version - be sure to call this in your class during interaction to allow Blueprint hooks*/
protected:	
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta=(DisplayName="On Interaction Started", BlueprintProtected))
		void BP_OnInteractionStarted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta = (DisplayName = "On Interaction Completed", BlueprintProtected))
		void BP_OnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta = (DisplayName = "On Interaction Canceled", BlueprintProtected))
		void BP_OnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context);
		
	
	UFUNCTION(BlueprintNativeEvent, Category = "Interactions")
		bool IsActionAvailable(const class AFirstPersonCharacter* User,const class UInteractiveComponent* InteractiveComponent, const class UInteractiveAction* Action) const;
	virtual bool IsActionAvailable_Implementation(const class AFirstPersonCharacter* User, const class UInteractiveComponent* InteractiveComponent, const class UInteractiveAction* Action) const
	{
		return true;
	};


	UInteractiveCollisionComponent* GetInteractiveComponent() const
	{
		const AActor* Owner = Cast<AActor>(this);
		return Owner ? Owner->FindComponentByClass<UInteractiveCollisionComponent>() : nullptr;
	}
};
