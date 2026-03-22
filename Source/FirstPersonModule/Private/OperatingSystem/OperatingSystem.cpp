// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystem.h"

AOperatingSystem::AOperatingSystem()
{
	Users.Add(FOperatingSystemUser("admin", "password2"));
	Users.Add(FOperatingSystemUser("Micah", "guest"));
	Users.Add(FOperatingSystemUser("Kik", "poopypants"));

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

void AOperatingSystem::SetCurrentUser(FOperatingSystemUser User)
{
	CurrentUser = User.Username;
}

TArray<FOperatingSystemUser> AOperatingSystem::GetAllUsers()
{
	return Users;
}

FString AOperatingSystem::GetCurrentUser()
{
	return CurrentUser;
}
