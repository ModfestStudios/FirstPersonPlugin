// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "Animations/Blends/FirstPersonStance.h"
#include "AnimNode_BlendListByStance.generated.h"

/** Selects a stance; the blend-list base handles transitions and pose evaluation. */
USTRUCT(BlueprintInternalUseOnly)
struct FIRSTPERSONMODULE_API FAnimNode_BlendListByStance : public FAnimNode_BlendListBase
{
	GENERATED_BODY()

	// Kept as runtime data so exposed graph inputs can update it on animation worker threads.
	UPROPERTY(EditAnywhere, Category = "Runtime", meta = (PinShownByDefault))
		EFirstPersonStance ActiveStance = EFirstPersonStance::Standing;

	//===============POSE SELECTION===============
	virtual int32 GetActiveChildIndex() override;

protected:
	//===============DEBUGGING===============
	virtual FString GetNodeName(FNodeDebugData& DebugData) override;
};
