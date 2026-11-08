// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_BlendListBase.h"
#include "Animations/Blends/AnimNode_BlendListByStance.h"
#include "AnimGraphNode_BlendListByStance.generated.h"

/** Fixed standing, crouched and prone inputs for the runtime stance blend node. */
UCLASS()
class FIRSTPERSONEDITORMODULE_API UAnimGraphNode_BlendListByStance : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Settings")
		FAnimNode_BlendListByStance Node;

	//===============INITIALIZATION===============
	UAnimGraphNode_BlendListByStance(const FObjectInitializer& ObjectInitializer);

	//===============GRAPH PRESENTATION===============
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
};
