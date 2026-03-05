// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractiveActorInterface.h"
#include "InteractiveActor.generated.h"

UCLASS(abstract)
class FIRSTPERSONMODULE_API AInteractiveActor : public AActor, public IInteractiveActorInterface
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UInteractiveCollisionComponent* InteractiveCollision;





	
public:	
	// Sets default values for this actor's properties
	AInteractiveActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void OnInteraction(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action) override;

};
