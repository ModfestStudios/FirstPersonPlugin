// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SensesEvaluationProfile.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API USensesEvaluationProfile : public UDataAsset
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		bool bHasVision = true;
	/*the maximum distance the AI can see - anything beyond this cannot be "visually" spotted*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		float MaxVisionDistance = 2400.0f;
	/*the minimum amount of time an actor must be within this AI's vision for them to spot them*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		FVector2D VisualDetectionDelayRange = FVector2D(0.35f, 0.75f);
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		float TimeBeforeSightReset = 2.0f;
	/*used to help calculate "awareness" of creatures within their range based on the max distance - with 1.0f having "immediate" awareness while lower numbers take longer for the AI to "register" something in their sight*/
	/*if a creature's vision is 100,then at 1.0f on the horizontal scale that'd be 100 - At 0.5f it'd be 50. Vertical scale is the "strength" of the vision. So if you keyed X/Z at (0.5, 1.0f) and (1.0f, 0.25f) then the AI's ability to spot something would "degrade" (take longer) between 50 and 100 units*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Vision")
		FRuntimeFloatCurve VisualFalloff;

	
};
