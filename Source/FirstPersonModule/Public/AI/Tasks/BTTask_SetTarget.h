// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetTarget.generated.h"

UENUM()
enum class ESetTargetType : uint8
{
	ClosestThreat,
	HighestThreat,	
	BlackboardKey
};

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UBTTask_SetTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Targetting")
		ESetTargetType SetTargetTo;


	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================

	UBTTask_SetTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const;



	
};
