// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystemApplication.h"
#include "OperatingSystem/OperatingSystem.h"

void AOperatingSystemApplication::LaunchApplication(const TArray<FString>& Parameters, AOperatingSystem* OS)
{
	OperatingSystem = OS;

	/*calls native C++ function*/
	OnApplicationLaunched(Parameters,OS);

	/*calls the Blueprint Hook*/
	BP_OnApplicationLaunched(Parameters);
}

void AOperatingSystemApplication::OnApplicationLaunched(const TArray<FString>& Parameters, AOperatingSystem* OS )
{
	//override this function to add custom logic
}

void AOperatingSystemApplication::CloseApplication()
{
	OperatingSystem = nullptr; //clear OS

	OnApplicationClosed();

	/*call the Blueprint hook*/
	BP_OnApplicationClosed();
}

void AOperatingSystemApplication::OnApplicationClosed()
{
	//override this function to add custom logic
}

AOperatingSystem* AOperatingSystemApplication::GetOperatingSystem()
{
	return OperatingSystem;
}

