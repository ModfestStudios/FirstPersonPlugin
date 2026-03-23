// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystem.h"

AOperatingSystem::AOperatingSystem()
{
	Users.Add(FOperatingSystemUser("admin", "password2"));
	Users.Add(FOperatingSystemUser("Micah", "guest"));
	Users.Add(FOperatingSystemUser("Kik", "poopypants"));

	Directories.Add(FOperatingSystemDirectory("Documents", 2, INDEX_NONE)); //INDEX_NONE = ROOT DIRECTORY
	Directories.Add(FOperatingSystemDirectory("Users", 3, INDEX_NONE));

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

TArray<FOperatingSystemDirectory> AOperatingSystem::GetAllRootDirectories()
{
	TArray<FOperatingSystemDirectory> RootDirs;

	for (FOperatingSystemDirectory& Dir : Directories)
	{
		if (Dir.ParentID == INDEX_NONE)
			RootDirs.Add(Dir);
	}

	return RootDirs;
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
