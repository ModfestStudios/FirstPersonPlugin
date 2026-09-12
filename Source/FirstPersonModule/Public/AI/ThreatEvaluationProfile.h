// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ThreatEvaluationProfile.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UThreatEvaluationProfile : public UDataAsset
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "Threat Scoring", meta = (InlineEditConditionToggle))
		bool bEvaluateDistance = false;
	UPROPERTY(EditAnywhere, Category = "Threat Scoring", meta = (EditCondition="bEvaluateDistance", ToolTip="X (Time) = Distance (cm) | Y (Value) = Threat Score"))
		FRuntimeFloatCurve DistanceScore;
};
