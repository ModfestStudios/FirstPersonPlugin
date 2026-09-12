// Copyrighted : Modfest Studios 2025-2026


#include "AI/Tasks/BTTask_Attack.h"
#include "AI/FirstPersonAIController.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = "Attack";

    BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Attack, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if(!Attack)
        return EBTNodeResult::Failed;
    
    AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());

    if(AIController)
        AIController->Attack(Attack);

    return EBTNodeResult::InProgress;
}

FString UBTTask_Attack::GetStaticDescription() const
{
    return FString();
}

void UBTTask_Attack::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
}
