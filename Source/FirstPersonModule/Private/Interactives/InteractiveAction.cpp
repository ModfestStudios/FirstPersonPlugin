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
	FunctionType = Config.FunctionType;
	FunctionToCall = Config.FunctionToCall;
}

void UInteractiveAction::NativeOnInteractionStarted(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User) const
{
	

	/*notify blueprint*/
	BP_OnInteractionStarted(InteractiveComponent, InteractiveActor, User);
}

void UInteractiveAction::NativeOnInteractionCompleted(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User) const
{

	/*notify blueprint*/
	BP_OnInteractionCompleted(InteractiveComponent,InteractiveActor,User);
}

void UInteractiveAction::NativeOnInteractionCanceled(UInteractiveComponent* InteractiveComponent, AActor* InteractiveActor, AFirstPersonCharacter* User, bool bInterrupted) const
{
	/*notify blueprint*/
	BP_OnInteractionCanceled(InteractiveComponent,InteractiveActor,User,bInterrupted);
}

bool UInteractiveAction::IsActionAvailable_Implementation(const UInteractiveComponent* InteractiveComponent,const AActor* InteractiveActor,const AFirstPersonCharacter* User) const
{	
	return true;
}




