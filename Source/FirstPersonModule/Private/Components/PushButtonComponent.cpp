// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PushButtonComponent.h"
#include "Components/BoxComponent.h"

#include "Net/UnrealNetwork.h"

UPushButtonComponent::UPushButtonComponent()
{
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	if (BaseMeshComponent)
	{
		BaseMeshComponent->SetupAttachment(GetAttachmentRoot());
	}

	ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Mesh"));
	if (ButtonMeshComponent)
	{
		ButtonMeshComponent->SetupAttachment(GetAttachmentRoot());
	}
	
	if (InteractiveCollision)
	{
		//InteractiveCollision->InitBoxExtent(FVector(4,4,4));
	}

	SetIsReplicatedByDefault(true);
}

#if WITH_EDITOR
void UPushButtonComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName =
		PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPushButtonComponent, BaseMesh))
	{
		if (BaseMeshComponent)
		{
			BaseMeshComponent->SetStaticMesh(BaseMesh);
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPushButtonComponent, ButtonMesh))
	{
		if (ButtonMeshComponent)
		{
			ButtonMeshComponent->SetStaticMesh(ButtonMesh);
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPushButtonComponent, BaseMeshTransform))
	{
		if (BaseMeshComponent)
		{
			BaseMeshComponent->SetRelativeTransform(BaseMeshTransform);
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPushButtonComponent, ButtonMeshTransform))
	{
		if (ButtonMeshComponent)
		{
			ButtonMeshComponent->SetRelativeTransform(ButtonMeshTransform);
		}
	}
}
#endif

void UPushButtonComponent::PushButton()
{	
	/*server*/
	if (GetNetMode() < NM_Client)
	{
		Multicast_PushButton();
	}
}

void UPushButtonComponent::Multicast_PushButton_Implementation()
{

}

void UPushButtonComponent::OnButtonPush()
{

}

UStaticMeshComponent* UPushButtonComponent::GetHoverStaticMesh()
{
	return ButtonMeshComponent;
}
