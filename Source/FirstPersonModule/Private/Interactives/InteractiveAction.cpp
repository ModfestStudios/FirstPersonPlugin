// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/InteractiveAction.h"
#include "Interactives/Interactives.h"
#include "Components/InteractiveComponent.h"


void UInteractiveAction::Initialize(class UInteractiveComponent* OwningComponent, const FInteractiveActionConfig& Config)
{
	ID = Config.ID;
	OwningInteractive = OwningComponent;
	
	if(Config.bOverrideActionLabel)
		InteractionLabel = Config.LabelOverride;
	if(InteractionLabel.IsEmpty())
	{
		FString ClassName = GetClass()->GetName();
		
		/*cleanup*/
		ClassName.RemoveFromEnd((TEXT("_C"))); //incase it's a blueprint class
		ClassName.RemoveFromEnd(TEXT("Action")); //subtract the "Action" keyword so we just get the verb

		/*convert to pretty name*/
		ClassName = FName::NameToDisplayString(ClassName,false);


		InteractionLabel = FText::FromString(ClassName);
	}
	

	bEnabled = Config.bEnabled;
	InteractionType = Config.InteractionType;
	ActionDuration = Config.ActionDuration;
    LongHeldActionDuration = Config.LongHeldActionDuration;
	FunctionType = Config.FunctionType;
	FunctionToCall = Config.FunctionToCall;
}

void UInteractiveAction::NativeOnInteractionStarted(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User, const FInteractionContext& Context) const
{
	

	/*notify blueprint*/
	BP_OnInteractionStarted(InteractiveComponent, InteractiveActor, User, Context);
}

void UInteractiveAction::NativeOnInteractionCompleted(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User, const FInteractionContext& Context) const
{

	/*notify blueprint*/
	BP_OnInteractionCompleted(InteractiveComponent,InteractiveActor,User,Context);
}

void UInteractiveAction::NativeOnInteractionCanceled(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User, const FInteractionContext& Context) const
{
	/*notify blueprint*/
	BP_OnInteractionCanceled(InteractiveComponent,InteractiveActor,User,Context);
}

bool UInteractiveAction::IsActionAvailable_Implementation(const UInteractiveComponent* InteractiveComponent,const AActor* InteractiveActor,const AFirstPersonCharacter* User) const
{	
	return true;
}




