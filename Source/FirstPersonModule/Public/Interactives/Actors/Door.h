// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactives/InteractiveActor.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "Door.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API ADoor : public AInteractiveActor
{
	GENERATED_BODY()
public:
	

	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//	class UStaticMeshComponent* Mesh;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	class UDoorComponent* DoorComponent;

	/*auto-initialized list of doors for this actor*/
	TArray<class UDoorComponent*> DoorComponents;

	//===============================================================================================================
	//===================================================FUNCTIONS===================================================
	//===============================================================================================================
	
public:	
	// Sets default values for this actor's properties
	ADoor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Interactive|Door")
		virtual void Open();
	UFUNCTION(BlueprintCallable, Category = "Interactive|Door")
		virtual void Close();	

	//===========================
	//==========ACTIONS==========
	//===========================
public:
	virtual void NativeOnInteractionStarted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual bool IsActionAvailable_Implementation(const AFirstPersonCharacter* User,const UInteractiveComponent* Interactive, const UInteractiveAction* Action) const override;


};
