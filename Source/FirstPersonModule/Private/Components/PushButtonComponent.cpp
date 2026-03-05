// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PushButtonComponent.h"

#include "Net/UnrealNetwork.h"

UPushButtonComponent::UPushButtonComponent()
{
	SetIsReplicatedByDefault(true);
}

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
