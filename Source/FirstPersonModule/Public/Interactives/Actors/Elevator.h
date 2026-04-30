// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactives/InteractiveActor.h"
#include "Elevator.generated.h"

USTRUCT(BlueprintType)
struct FElevatorDestination
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destination")
		FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destination")
		FName DestinationId = NAME_None;

	FElevatorDestination() {};
	FElevatorDestination(FName Id, FVector StartLocation) : DestinationId(Id), Location(StartLocation) {};	
};


/**
 * 
 */
UCLASS(Blueprintable, HideCategories = ("Transform", "Actor Tick", "Static Mesh", "Component Tick", "Physics", "Constraints", "LOD", "Collision", "Lighting", "Asset Uset Data", "Replication", "Component Replication","Activation", "Navigation", "Mesh Painting", "Material Parameters", "HLOD", "Mobile", "Input"))
class FIRSTPERSONMODULE_API AElevator : public AInteractiveActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Elevator")
		float ElevatorSpeed = 80.0f;
	/*threshold that'll trigger the "Stop" of the elevator automatically if we reach around here*/
	UPROPERTY(EditAnywhere, Category = "Elevator")
		float StoppingThreshold = 2.5f;


	UPROPERTY()
		bool bElevatorMoving = false;
	UPROPERTY(EditAnywhere, Category = "Elevator")
		FName StartingDestination = "G";
private:
	UPROPERTY()
		FVector StartingLocation = FVector(0);	
	UPROPERTY()
		FName CurrentDestination;
	UPROPERTY()
		FName NextDestination;	

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Elevator")
		class USceneComponent* SceneComponent;	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Elevator")
		class UStaticMeshComponent* LeftDoor;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Elevator")
		class UStaticMeshComponent* RightDoor;
	UPROPERTY(BlueprintReadOnly, Category = "Elevator")
		TArray<FElevatorDestination> Destinations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elevator|Destinations")
		class USceneComponent* DestinationsComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elevator|Destinations")
		class UElevatorDestinationComponent* StartingDestinationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
	class UStaticMeshComponent* Mesh;

	///========================================================================================================================
	//========================================================FUNCTIONS========================================================
	///========================================================================================================================
public:

	AElevator(const FObjectInitializer& ObjectInitailizer);

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnDestinationComponentAdded(class UElevatorDestinationComponent* Destination);



	//===========================
	//=====ELEVATOR MOVEMENT=====
	//===========================
	
	UFUNCTION(BlueprintCallable, Category = "Elevator")
		virtual void StartElevator();
	UFUNCTION(BlueprintCallable, Category = "Elevator")
		virtual void StopElevator();

	UFUNCTION(BlueprintCallable, Category = "Elevator|Destinations")
		virtual void SnapToDestination(FName Destination);
	UFUNCTION(BlueprintCallable, Category = "Elevator|Destinations")
		virtual void SetDestination(FName Destination);


};
