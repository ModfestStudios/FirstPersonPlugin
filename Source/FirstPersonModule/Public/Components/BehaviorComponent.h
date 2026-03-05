// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorComponent.generated.h"


UCLASS( ClassGroup=(ArtificialIntelligences), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Behavior Tree")
		class UBehaviorTree* BehaviorTree;



	//=====================================================================================================================================================================================
	//======================================================================================FUNCTIONS======================================================================================
	//=====================================================================================================================================================================================

public:	
	// Sets default values for this component's properties
	UBehaviorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION()
		virtual void InitBehaviorTree();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
