// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	/*called by the InteractiveComponent whenever someone tries to use this*/
	
	virtual void OnInteraction(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action)
	{
		Execute_BP_OnInteraction(Cast<AActor>(this), User, InteractiveComponent, Action);
	}
	
	/*blueprint version - be sure to call this in your class during interaction to allow Blueprint hooks*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta=(DisplayName="On Interaction"))
		void BP_OnInteraction(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action);

	

	UFUNCTION(BlueprintNativeEvent, Category = "Interactions")
		void OnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Interactions")
		void OnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Interactions")
		bool IsInteractionAvailable(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action);
	virtual bool IsInteractionAvailable_Implementation(AFirstPersonCharacter* User, UInteractiveCollisionComponent* InteractiveComponent, const UInteractiveAction* Action)
	{
		return true;
	}

	UInteractiveCollisionComponent* GetInteractiveComponent() const
	{
		const AActor* Owner = Cast<AActor>(this);
		return Owner ? Owner->FindComponentByClass<UInteractiveCollisionComponent>() : nullptr;
	}
};
