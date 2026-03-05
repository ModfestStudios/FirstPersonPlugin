// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ElevatorDestinationComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interactives), HideCategories = ("Sockets", "Navigation", Rendering, Physics, "Component Tick"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UElevatorDestinationComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:

		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destination")
		FName DestinationId;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	virtual void OnComponentCreated() override;
	
#endif


	//=================================================================================================================================
	//=============================================================FUNCTIONS===========================================================
	//=================================================================================================================================
	
	UElevatorDestinationComponent(const FObjectInitializer& ObjectInitializer);


};
