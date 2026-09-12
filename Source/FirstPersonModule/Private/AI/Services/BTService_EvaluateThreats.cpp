// Copyrighted : Modfest Studios 2025-2026


#include "AI/Services/BTService_EvaluateThreats.h"
#include "AI/FirstPersonAIController.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/BehaviorComponent.h"

UBTService_EvaluateThreats::UBTService_EvaluateThreats()
{
	NodeName = "Evaluate Threats";
	bNotifyTick = true;
	bCreateNodeInstance = false;
}

void UBTService_EvaluateThreats::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	/*initialize*/
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	AFirstPersonCharacter* Pawn = AIController ? AIController->GetPawn<AFirstPersonCharacter>() : nullptr;
	UBehaviorComponent* BehaviorComponent = Pawn ? Pawn->GetBehaviorComponent() : nullptr;

	if(!AIController || !Pawn || !BehaviorComponent)
		return;

	BehaviorComponent->EvaluateThreats(ThreatProfile, DeltaSeconds);
}

void UBTService_EvaluateThreats::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
}

void UBTService_EvaluateThreats::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
}
