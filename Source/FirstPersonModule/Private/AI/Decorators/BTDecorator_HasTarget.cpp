// Copyrighted : Modfest Studios 2025-2026


#include "AI/Decorators/BTDecorator_HasTarget.h"
#include "AI/FirstPersonAIController.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/SensesComponent.h"

UBTDecorator_HasTarget::UBTDecorator_HasTarget()
{
	//bShowInverseConditionDesc = true;
}

bool UBTDecorator_HasTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	/*initialize*/
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	
	if(!AIController)
		return false;
	else
		return AIController->HasTarget();
}

FString UBTDecorator_HasTarget::GetStaticDescription() const
{	
	if(IsInversed())
		return FString("Does NOT Have a Target");
	else
		return FString("Has a Target");
}

void UBTDecorator_HasTarget::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	/*initialize*/
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	FString TargetString = (AIController && AIController->HasTarget()) ? "Target: " + GetNameSafe(AIController->GetTarget()) : "Target: None";
	
	Values.Add(TargetString);
	
}
