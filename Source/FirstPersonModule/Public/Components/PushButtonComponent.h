// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InteractiveComponent.h"
#include "PushButtonComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = ("Interactives"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UPushButtonComponent : public UInteractiveComponent
{
	GENERATED_BODY()
public:


	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================

	UPushButtonComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Push Button")
		void PushButton();
	
protected:
	/*multicast-event that notifies all clients (and ran on server) that the button was pressed*/
	UFUNCTION(Reliable, NetMulticast)
		void Multicast_PushButton();
	
	/*[Server/Clients] native C++ butotn pressed event*/
	UFUNCTION()
		virtual void OnButtonPush();
	/*[Server/Clients] blueprint event that's called whenever the button is pressed*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Push Button", meta = (DisplayName="OnButtonPush"))
		void BP_OnButtonPush();

	
};
