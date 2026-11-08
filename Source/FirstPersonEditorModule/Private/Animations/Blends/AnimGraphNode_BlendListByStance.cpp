// Copyrighted : Modfest Studios 2025-2026

#include "Animations/Blends/AnimGraphNode_BlendListByStance.h"
#include "EdGraph/EdGraphPin.h"

#define LOCTEXT_NAMESPACE "FirstPersonStanceBlend"

//===============INITIALIZATION===============
UAnimGraphNode_BlendListByStance::UAnimGraphNode_BlendListByStance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// The order matches GetActiveChildIndex(). Each pose starts with a 0.1 second blend.
	Node.AddPose();
	Node.AddPose();
	Node.AddPose();
}

//===============GRAPH PRESENTATION===============
FText UAnimGraphNode_BlendListByStance::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Poses by Stance");
}

FText UAnimGraphNode_BlendListByStance::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blends between standing, crouched and prone poses using Active Stance. Each blend time controls the transition into that stance.");
}

void UAnimGraphNode_BlendListByStance::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	FText StanceName;

	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);

	if (SourcePropertyName != TEXT("BlendPose") && SourcePropertyName != TEXT("BlendTime"))
		return;

	switch (ArrayIndex)
	{
	case 0:
		StanceName = LOCTEXT("Standing", "Standing");
		break;
	case 1:
		StanceName = LOCTEXT("Crouched", "Crouched");
		break;
	case 2:
		StanceName = LOCTEXT("Prone", "Prone");
		break;
	default:
		return;
	}

	if (SourcePropertyName == TEXT("BlendPose"))
		Pin->PinFriendlyName = FText::Format(LOCTEXT("PosePin", "{0} Pose"), StanceName);
	else
		Pin->PinFriendlyName = FText::Format(LOCTEXT("TimePin", "{0} Blend Time"), StanceName);
}

#undef LOCTEXT_NAMESPACE
