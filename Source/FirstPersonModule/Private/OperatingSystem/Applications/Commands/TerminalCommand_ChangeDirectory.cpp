// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_ChangeDirectory.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

//TODO: KiK needs to clean this up; works for now
FTerminalCommandResult UTerminalCommand_ChangeDirectory::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
	if (CommandParameters.Subcommands.Num() != 1) {
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
		return FTerminalCommandResult();
	}
	else {
		FString DirectoryRequested = CommandParameters.Subcommands[0].Subcommand.ToString();
		if (DirectoryRequested.Right(1) == "/") {
			DirectoryRequested = DirectoryRequested.LeftChop(1);
		}
		AOperatingSystem* OS = Terminal->GetOperatingSystem();
		FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
		FString HomeDirectory = OS->GetHomeDirectory();
		TArray<FString> DirectoryRequestedParsing;
		bool PrefixedHomeDirectory = false;
		bool DirectoryExists = false;
		FString NewWorkingDirectory;
		int32 HomeDirectoryLength = HomeDirectory.Len() + 2;

		if (DirectoryRequested.Left(1) == "/") {
			return FTerminalCommandResult(false, "No such directory: " + DirectoryRequested, false);
		}
		if (DirectoryRequested.Contains("/")) {

			//Allows for "straight to home" by typing "cd ~/homedirectory" 
			if (DirectoryRequested.Left(HomeDirectoryLength) == ("~/" + HomeDirectory)) {
				if (DirectoryRequested == ("~/" + HomeDirectory)) {
					OS->SetActiveWorkingDirectory("");
					return FTerminalCommandResult();
				}
				PrefixedHomeDirectory = true;
			}

			DirectoryRequested.ParseIntoArray(DirectoryRequestedParsing, TEXT("/"), true);
			FString DirectoryPathBuilding = "";
			int32 LastDirectoryNum = DirectoryRequestedParsing.Num() - 1;
			//Isolate intended directory
			FString RequestedDirectory = DirectoryRequestedParsing[LastDirectoryNum];
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Looking for directory :" + RequestedDirectory));
			//Isolate path to intended directory
			for (int32 i = 0; i < DirectoryRequestedParsing.Num(); i++) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("CurrentParse :" + DirectoryRequestedParsing[i]));
				if (DirectoryRequestedParsing[i] == HomeDirectory || DirectoryRequestedParsing[i] == "~")
				{
					DirectoryPathBuilding = "";
					continue;
				}
				if (i != LastDirectoryNum) {
					DirectoryPathBuilding = DirectoryPathBuilding + "/" + DirectoryRequestedParsing[i];
				}
			}
			DirectoryExists = OS->FileSystemCheckIfFileExists(RequestedDirectory, DirectoryPathBuilding, FOperatingSystemFileType::Directory);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Check for PATH of " + DirectoryPathBuilding + " and file of " + RequestedDirectory));
			if (!DirectoryExists) {
				return FTerminalCommandResult(false, "No such directory: " + DirectoryRequested, false);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Made it to OS SetWorkingDirectory"));
				OS->SetActiveWorkingDirectory(DirectoryRequested);
				return FTerminalCommandResult();
			}
		}
		//TODO: This assumes we only let one child directory down from ~/lambda5 ; will need changed if we intended to do children of children directories too
		if (DirectoryRequested == "..") {
			if (ActiveWorkingDirectory.IsEmpty()) {
				Terminal->PrintToTerminal("bash: cd: /: Permission denied");
				return FTerminalCommandResult();
			}
			else {
				if (ActiveWorkingDirectory.Contains("/")) {
					ActiveWorkingDirectory.ParseIntoArray(DirectoryRequestedParsing, TEXT("/"), true);
					if (DirectoryRequestedParsing.Num() == 1) {
						OS->SetActiveWorkingDirectory("");
						return FTerminalCommandResult();
					}
				}
			}
		}
		else
		{
			DirectoryExists = OS->FileSystemCheckIfFileExists(DirectoryRequested, ActiveWorkingDirectory, FOperatingSystemFileType::Directory);
			if (!DirectoryExists) { return FTerminalCommandResult(false, "No such directory: " + DirectoryRequested, false); }
			else { OS->SetActiveWorkingDirectory("/" + DirectoryRequested); }
		}
	}
	return FTerminalCommandResult();
}

UTerminalCommand_ChangeDirectory::UTerminalCommand_ChangeDirectory()
{
	Command = "cd";
}
