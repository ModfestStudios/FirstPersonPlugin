// Copyrighted : Modfest Studios 2025-2026

#include "Animations/Blends/AnimNode_BlendListByStance.h"

//===============POSE SELECTION===============
int32 FAnimNode_BlendListByStance::GetActiveChildIndex()
{
	// Explicit mapping also gives invalid enum values a safe standing fallback.
	switch (ActiveStance)
	{
	case EFirstPersonStance::Crouched:
		return 1;
	case EFirstPersonStance::Prone:
		return 2;
	case EFirstPersonStance::Standing:
	default:
		return 0;
	}
}

//===============DEBUGGING===============
FString FAnimNode_BlendListByStance::GetNodeName(FNodeDebugData& DebugData)
{
	return DebugData.GetNodeName(this);
}
