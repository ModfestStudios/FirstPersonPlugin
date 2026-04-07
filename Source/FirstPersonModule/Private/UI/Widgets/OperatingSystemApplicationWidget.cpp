// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/OperatingSystemApplicationWidget.h"
#include "OperatingSystem/OperatingSystem.h"
#include "OperatingSystem/OperatingSystemApplication.h"


void UOperatingSystemApplicationWidget::NativeOnInitialized()
{
	if (bCreateApplicationOnInitialize && IsValid(ApplicationClass))
	{
		if (GetWorld() && GetWorld()->GetNetMode() < NM_Client)
		{
			AOperatingSystem* OS = GetWorld()->SpawnActor<AOperatingSystem>(OperatingSystemClass);

			Application = GetWorld()->SpawnActor<AOperatingSystemApplication>(ApplicationClass);
			
			if (Application)
				Application->LaunchApplication(ApplicationLaunchParameters, OS);
		}
	}


	Super::NativeOnInitialized();
}
