// Copyrighted : Modfest Studios 2025-2026


#include "AI/Decorators/BTDecorator_HasDetectedActor.h"
#include "AI/FirstPersonAIController.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/SensesComponent.h"

UBTDecorator_HasDetectedActor::UBTDecorator_HasDetectedActor()
{
	NodeName = "Has Detected Actor";
	//bNotifyTick = true;
	//bCreateNodeInstance = false;
}

bool UBTDecorator_HasDetectedActor::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	/*initialize*/
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	AFirstPersonCharacter* Pawn = AIController ? AIController->GetPawn<AFirstPersonCharacter>() : nullptr;
	USensesComponent* SensesComponent = Pawn ? Pawn->GetSensesComponent() : nullptr;

	/*safety check*/
	if (!AIController || !Pawn || !SensesComponent)
		return false;

	int32 NumActorsDetected = SensesComponent->GetNumberOfDetectedActors();
	
	if(DetectionCriteria == EActorDetectionCriteria::Any && NumActorsDetected > 0)
		return true;
	if(DetectionCriteria == EActorDetectionCriteria::EqualTo && NumActorsDetected == NumberOfActors)
		return true;
	if(DetectionCriteria == EActorDetectionCriteria::LessThan && NumActorsDetected > 0 && (NumActorsDetected < NumberOfActors))
		return true;
	if(DetectionCriteria == EActorDetectionCriteria::GreaterThan && NumActorsDetected > NumberOfActors)
		return true;

	/*otherwise return false*/
	return false;
}

FString UBTDecorator_HasDetectedActor::GetStaticDescription() const
{
	if(DetectionCriteria == EActorDetectionCriteria::Any)
		return "Has Detected Any Actor";

	if(DetectionCriteria == EActorDetectionCriteria::EqualTo)
		return "Detected " + FString::FromInt(NumberOfActors) + " Actor(s)";

	if(DetectionCriteria == EActorDetectionCriteria::LessThan)
		return "Has Detected Less Than " + FString::FromInt(NumberOfActors) + " Actors";

	if(DetectionCriteria == EActorDetectionCriteria::GreaterThan)
		return "Has Detected Greater Than " + FString::FromInt(NumberOfActors) + " Actors";

	else
		return "";
	
}

void UBTDecorator_HasDetectedActor::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	AFirstPersonAIController* AIController = Cast<AFirstPersonAIController>(OwnerComp.GetAIOwner());
	AFirstPersonCharacter* Pawn = AIController ? AIController->GetPawn<AFirstPersonCharacter>() : nullptr;
	USensesComponent* SensesComponent = Pawn ? Pawn->GetSensesComponent() : nullptr;

	if(!AIController || !Pawn || !SensesComponent)
		return;

	int32 NumActorsDetected = SensesComponent->GetNumberOfDetectedActors();
	FString RuntimeString = "Number of Detected Actors: " + FString::FromInt(NumActorsDetected);

	Values.Add(RuntimeString);	
}
