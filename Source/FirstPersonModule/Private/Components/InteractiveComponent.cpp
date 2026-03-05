// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractiveComponent.h"

/*components*/

/*engine*/
#include "Engine/StaticMesh.h"

/*materials*/
#include "Materials/MaterialInstanceDynamic.h"

#include "../Logging.h"
// Sets default values for this component's properties
UInteractiveComponent::UInteractiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();
	
	/*initiate dynamic material*/
	if (bHighlightOnHover && DynamicMaterial == nullptr && GetStaticMesh() != nullptr)
	{
		if (UMaterialInterface* DefaultMaterial = GetStaticMesh()->GetMaterial(0))
		{
			DynamicMaterial = CreateDynamicMaterialInstance(0, DefaultMaterial,FName("DynamicButtonMaterial"));
		}
	}	
}


// Called every frame
void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractiveComponent::Interact(AFirstPersonCharacter* User)
{
	if (bAutoCallFunction)
		CallOwnerFunctionByName();

	NativeOnInteraction(User);

	/*blueprint version*/
	BP_OnInteraction(User);

	if (OnInteraction.IsBound())
		OnInteraction.Broadcast(User);
}

void UInteractiveComponent::NativeOnInteraction(class AFirstPersonCharacter* User)
{
}


void UInteractiveComponent::CallOwnerFunctionByName()
{
	
}

void UInteractiveComponent::OnHovered(AFirstPersonCharacter* User)
{
	if (DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(HighlightMaterialParameterName, HighlightMaterialIntensity);

	//BP_OnButtonHovered(User);
}

void UInteractiveComponent::OnUnhovered(AFirstPersonCharacter* User)
{
	if (DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(HighlightMaterialParameterName, DefaultMaterialIntensity);

	//BP_OnButtonUnhovered(User);
}

