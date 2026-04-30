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
		AOperatingSystem* OS = Terminal->GetOperatingSystem();
		FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
		FString HomeDirectory = OS->GetHomeDirectory();
		TArray<FString> DirectoryRequestedParsing;
		bool PrefixedHomeDirectory = false;
		bool DirectoryExists = false;
		FString NewWorkingDirectory;
		int32 HomeDirectoryLength = HomeDirectory.Len() + 2;
		
		if (DirectoryRequested.Contains("/")) {
			FString DirectoryPathPrefix = "";
			if (DirectoryRequested.Left(HomeDirectoryLength) == ("~/" + HomeDirectory) || DirectoryRequested == "/") {
				// Allows user to go straight to home if they just type ~/{homedirectory} in CD command
				if (DirectoryRequested.Len() == HomeDirectoryLength || DirectoryRequested == "/") {
					OS->SetActiveWorkingDirectory("");
					return FTerminalCommandResult();
				}
				PrefixedHomeDirectory = true;
			}
			else {
				DirectoryPathPrefix = "/";
			}

			DirectoryRequested.ParseIntoArray(DirectoryRequestedParsing, TEXT("/"), true);
			FString DirectoryCount = FString::FromInt(DirectoryRequestedParsing.Num());
			int32 LastDirectoryNum = DirectoryRequestedParsing.Num() - 1;
			FString RequestedDirectory = DirectoryRequestedParsing[LastDirectoryNum];
			FString DirectoryPathBuilding = DirectoryPathPrefix + "";
			
			if (RequestedDirectory.Left(1) == ".") {
				RequestedDirectory = RequestedDirectory.RightChop(1);
			}
			
			//Isolate path to intended directory
			for (int32 i = 0; i < DirectoryRequestedParsing.Num(); i++) {
				if ((i == 0 || i == 1) && PrefixedHomeDirectory) {
					continue;
				}
				if (i != LastDirectoryNum) {
					DirectoryPathBuilding = DirectoryPathBuilding + "/" + DirectoryRequestedParsing[i];
				}
			}
			DirectoryExists = OS->FileSystemCheckIfFileExists(RequestedDirectory, DirectoryPathBuilding, FOperatingSystemFileType::Directory);
			if (!DirectoryExists) {
				return FTerminalCommandResult(false, "No such directory: " + DirectoryRequested, false);
			}
			else
			{
				if (DirectoryPathBuilding == "") {
					DirectoryPathBuilding = "/";
				}
				OS->SetActiveWorkingDirectory(DirectoryPathBuilding + RequestedDirectory);
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
			if (DirectoryRequested.Left(1) == ".") {
				DirectoryRequested = DirectoryRequested.RightChop(1);
			}
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
