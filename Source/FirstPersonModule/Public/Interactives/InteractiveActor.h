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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
		class UInteractiveComponent* InteractiveComponent;



	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
	//	class UInteractiveCollisionComponent* InteractiveCollision;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
	//	class UInventoryManagerComponent* InventoryManager;





	
public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	virtual void NativeOnInteractionStarted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCompleted(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual void NativeOnInteractionCanceled(class AFirstPersonCharacter* User, class UInteractiveComponent* Interactive, const class UInteractiveAction* Action, const FInteractionContext& Context) override;
	virtual bool IsActionAvailable_Implementation(const AFirstPersonCharacter* User, const UInteractiveComponent* Interactive, const UInteractiveAction* Action) const override;


public:
	/*returns whether true if this user is currently interacting with this Actor*/
	UFUNCTION()
		virtual bool IsBeingInteractedBy(class AFirstPersonCharacter* User);


};
