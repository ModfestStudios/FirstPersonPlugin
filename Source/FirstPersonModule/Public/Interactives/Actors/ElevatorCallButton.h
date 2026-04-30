// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactives/InteractiveActor.h"
#include "Components/InteractiveCollisionComponent.h"
#include "ElevatorCallButton.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AElevatorCallButton : public AInteractiveActor
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintAssignable, Category = "Interactions")
		FInteractionAction OnElevatorCalled;

	UPROPERTY(VisibleAnywhere, Category = "Interactions")
		class UPushButtonComponent* PushButton;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UInteractiveCollisionComponent* InteractiveCollision;

	//=======================================================================================================================================================
	//=======================================================================FUNCTIONS=======================================================================
	//=======================================================================================================================================================

	AElevatorCallButton(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
		virtual void OnButtonPressed(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);
	UFUNCTION()
		virtual void OnButtonHovered(class AFirstPersonCharacter* User);
	UFUNCTION()
		virtual void OnButtonUnhovered(class AFirstPersonCharacter* User);

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "On Elevator Called"))
		void BP_OnElevatorCalled(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);

	
};
