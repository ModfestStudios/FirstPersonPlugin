// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystem.h"

AOperatingSystem::AOperatingSystem()
{

	bReplicates = true;
}

void AOperatingSystem::Boot()
{
	if (SystemState == EOperatingSystemMainState::PoweredOff)
		BeginBootSequence();
}

void AOperatingSystem::BeginBootSequence()
{

}

void AOperatingSystem::InitiateBIOS()
{

}

void AOperatingSystem::EndBootSequence()
{
}
