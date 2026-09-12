// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactives/Interactives.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "InteractiveActor.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API AInteractiveActor : public AActor, public IInteractiveActorInterface
{
	GENERATED_BODY()
public:


	
protected:
	/*list of people actively using this item*/
	UPROPERTY(Replicated)
		TArray<class AFirstPersonCharacter*> ActiveUsers;



public:
	/*components*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "Interactive")
		class UInteractiveComponent* InteractiveComponent;



	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
	//	class UInteractiveCollisionComponent* InteractiveCollision;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
	//	class UInventoryManagerComponent* InventoryManager;





	
public:	
	// Sets default values for this actor's properties
	AInteractiveActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//UFUNCTION()
	//	virtual void NativeOnInteraction();


protected:
	UFUNCTION()
		virtual void NativeOnInteractionBegins(class AFirstPersonCharacter* User, const class UInteractiveAction* Action);
	UFUNCTION()
		virtual void NativeOnInteractionEnds(class AFirstPersonCharacter* User, const class UInteractiveAction* Action, EInteractionEndReason EndReason);
	//UFUNCTION()
	//	virtual void OnInteraction(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action) override;
	//UFUNCTION()
	//	virtual void OnInteractionEnds(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action);
	/*UFUNCTION()
		virtual void OnInteractionInterrupted()*/
public:
	/*returns whether true if this user is currently interacting with this Actor*/
	UFUNCTION()
		virtual bool IsBeingInteractedBy(class AFirstPersonCharacter* User);


};
