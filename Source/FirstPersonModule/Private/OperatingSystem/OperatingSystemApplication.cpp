// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystemApplication.h"
#include "OperatingSystem/OperatingSystem.h"

void AOperatingSystemApplication::OnApplicationLaunched(const TArray<FString>& Parameters, AOperatingSystem* OS )
{
	OperatingSystem = OS;

	/*calls the Blueprint Hook*/
	BP_OnApplicationLaunched(Parameters);
}

void AOperatingSystemApplication::OnApplicationClosed()
{

	/*calls the Blueprint Hook*/
	BP_OnApplicationClosed();
}

AOperatingSystem* AOperatingSystemApplication::GetOperatingSystem()
{
	
	return nullptr;
}

