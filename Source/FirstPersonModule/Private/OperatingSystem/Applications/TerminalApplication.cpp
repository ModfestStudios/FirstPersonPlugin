// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_ParseCommand.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Clear.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Echo.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_WhoAmI.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_List.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_qrscanner.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_touch.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_gpg.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_rm.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_ChangeDirectory.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Help.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_HelloWorld.h"
#include "OperatingSystem/Applications/Commands/TerminalCommand_Exit.h"


ATerminalApplication::ATerminalApplication()
{
	Commands.Add(UTerminalCommand_ParseCommand::StaticClass());
	Commands.Add(UTerminalCommand_Clear::StaticClass());
	Commands.Add(UTerminalCommand_Echo::StaticClass());
	Commands.Add(UTerminalCommand_WhoAmI::StaticClass());
	Commands.Add(UTerminalCommand_List::StaticClass());
	Commands.Add(UTerminalCommand_qrscanner::StaticClass());
	Commands.Add(UTerminalCommand_touch::StaticClass());
	Commands.Add(UTerminalCommand_gpg::StaticClass());
	Commands.Add(UTerminalCommand_rm::StaticClass());
	Commands.Add(UTerminalCommand_ChangeDirectory::StaticClass());
	Commands.Add(UTerminalCommand_Help::StaticClass());
	Commands.Add(UTerminalCommand_HelloWorld::StaticClass());
	Commands.Add(UTerminalCommand_Exit::StaticClass());

	PrimaryActorTick.bCanEverTick = true;

}

void ATerminalApplication::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessDelayedMessages(DeltaTime);
}

bool ATerminalApplication::IsMessageQueued()
{
	if (MessageQueue.Num() > 0)
		return true;
	else
		return false;
}

void ATerminalApplication::ProcessDelayedMessages(float DeltaTime)
{
	if (IsMessageQueued())
	{
		/*always grab only the FIRST message and modify it, making all others paused until this one is complete*/
		FDelayedTerminalMessage& QueuedMessage = MessageQueue[0];
		float TimeRemaining = QueuedMessage.DelayRemaining - DeltaTime; //Tick() is per-frace, while DeltaTime is time (in seconds) since last frame/tick. Used for tracking time-passed with variable frame rates
		MessageQueue[0].DelayRemaining = TimeRemaining;
		
		/*message queue has been completed - and we can now fire off*/
		if (TimeRemaining <= 0.0f)
		{
			PrintMessage(QueuedMessage.Message);
			MessageQueue.RemoveAt(0); //removes the message, and shrinks everything into place for next round
		}
	}
}

void ATerminalApplication::ExecuteCommand(FString Command)
{
	/*initialize*/
	TArray<FString> CommandArray;
	int32 CommandIndex = -1;
	FTerminalCommandExecutionParameters CommandParameters;
	AOperatingSystem* OS = this->GetOperatingSystem();
	FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
	FString HomeDirectory = OS->GetHomeDirectory();

	/*save command to history*/
	AddCommandToHistory(Command);
	ResetCommandHistory();

	/*print entered command to terminal*/
	PrintToTerminal("<User>white-rabbit</>:<directory>~/" + HomeDirectory + ActiveWorkingDirectory + "</>$ <Default>" + Command + "</>");

	/*split everything up based on spaces*/
	Command.ParseIntoArray(CommandArray,TEXT(" "), true);

	/*do some parsing*/
	ParseQuotes(CommandArray);
	ParseCommands(CommandArray, CommandParameters);	
	
	/*execute commands*/
	if (BlockedCommand(CommandParameters.Command.ToString())) {
		PrintToTerminal("System: " + CommandParameters.Command.ToString() + ": Permission denied: Command forbidden");
		PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
	}
	else
	{
		if (IsValidCommand(CommandParameters.Command, CommandIndex) && CommandIndex >= 0)
		{
			ExecuteCommandObject(Commands[CommandIndex], CommandParameters);
		}
		else
		{
			PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, *CommandParameters.Command.ToString());
			PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
		}
	}
}


bool ATerminalApplication::IsValidCommand(FName Command, int32& CommandIndex)
{
	CommandIndex = -1; //default

	for (int32 i = 0; i < Commands.Num(); i++)
	{
		if (Commands[i] && Commands[i]->GetDefaultObject<UTerminalCommand>()->Command == Command)
		{
			CommandIndex = i;
			return true;
		}			
	}

	return false;
}

void ATerminalApplication::ExecuteCommandObject(TSubclassOf<UTerminalCommand> Command,FTerminalCommandExecutionParameters CommandParameters)
{
	if (Command)
	{
		FTerminalCommandResult Result = Command->GetDefaultObject<UTerminalCommand>()->ExecuteCommand(this, CommandParameters);

		if (!Result.TerminalMessage.IsEmpty())
		{
			if (Result.bSuccess)
			{
				PrintToTerminal(Result.TerminalMessage);
			}
			else
			{
				PrintToTerminal(Result.TerminalMessage, ETerminalMessageStyle::Error);
			}
		}
	}
}

void ATerminalApplication::PrintToTerminal(const FString& Message, ETerminalMessageStyle Style, float Delay)
{
	FString MessageToPrint = FString();

	switch (Style)
	{
		case ETerminalMessageStyle::None:
			MessageToPrint = Message;
			break;
		case ETerminalMessageStyle::OK:
			MessageToPrint = "<OK>[OK]</> " + Message;
			break;
		case ETerminalMessageStyle::Error:
			MessageToPrint = "<ERROR>[ERROR]</> " + Message;
			break;
		case ETerminalMessageStyle::Status:
			MessageToPrint = "<Status>[STATUS]</> " + Message;
			break;
	}

	/*if the message has a delay, add it to the queue*/
	if (Delay > 0.0f)
		MessageQueue.Add(FDelayedTerminalMessage(MessageToPrint, Delay));
	/*if the message does not have a delay, but delays are still actively running - we need to add it to the queue with a 0.0f delay to wait its turn*/
	else if (MessageQueue.Num() > 0)
			MessageQueue.Add(FDelayedTerminalMessage(MessageToPrint, 0.0f));
	/*if there is no delay, and currently the queue is not backed up - then we just print directly*/
	else 
		PrintMessage(MessageToPrint);

	
}

void ATerminalApplication::PrintCommonTerminalResponse(ETerminalCommonMessage MessageResponse, FString UserDefinedValue1)
{
	//TODO: Need to cleanup common errors to properly mimic linux common failures;
	switch (MessageResponse)
	{
	case ETerminalCommonMessage::SyntaxLength:
		PrintToTerminal("Syntax too long/short " + UserDefinedValue1 + " ; for further help try appending --help to command/tool", ETerminalMessageStyle::Error);
		break;
	case ETerminalCommonMessage::SyntaxFormat:
		PrintToTerminal("Syntax error invalid format  " + UserDefinedValue1 + " ; for further help try appending --help to command/tool", ETerminalMessageStyle::Error);
		break;
	case ETerminalCommonMessage::CommandNotFound:
		PrintToTerminal(UserDefinedValue1 + ": command not found");
		break;
	case ETerminalCommonMessage::UseHelp:
		PrintToTerminal("For commands available, use: help");
		break;
	}
}

bool ATerminalApplication::CheckToolExists(FString inToolName)
{
	AOperatingSystem* OS = this->GetOperatingSystem();
	FOperatingSystemFiles FileResult;
	FString ToolName = "Tool:" + inToolName;

	FileResult = OS->FileSystemGetFile("", "", ToolName);

	if (FileResult.Name != "") {
		return true;
	}

	return false;
}

void ATerminalApplication::PrintMessage(FString Message)
{	
	TerminalMessages.Add(Message);
	RefreshTerminal();
}

void ATerminalApplication::ClearTerminal()
{
	TerminalMessages.Reset();

	RefreshTerminal();
}

void ATerminalApplication::RefreshTerminal()
{
	if (OnTerminalUpdated.IsBound())
		OnTerminalUpdated.Broadcast();
}

void ATerminalApplication::ParseQuotes(TArray<FString>& CommandArray)
{
	/* combine quoted commands into a single entry */
	for (int32 i = 0; i < CommandArray.Num(); i++)
	{
		FString& Token = CommandArray[i];

		if (!Token.StartsWith(TEXT("\"")))
		{
			continue;
		}


		if (Token.Len() >= 2 && Token.EndsWith(TEXT("\"")))
		{
			Token = Token.Mid(1, Token.Len() - 2);
			continue;
		}

		// Start combining
		FString Combined = Token.RightChop(1); // remove opening quote
		int32 EndIndex = i;
		bool bFoundClosingQuote = false;

		for (int32 NextIndex = i + 1; NextIndex < CommandArray.Num(); ++NextIndex)
		{

			Combined += TEXT(" ");
			Combined += CommandArray[NextIndex];

			if (CommandArray[NextIndex].EndsWith(TEXT("\"")))
			{
				// Remove closing quote
				Combined.LeftChopInline(1);

				EndIndex = NextIndex;
				bFoundClosingQuote = true;
				break;
			}
		}

		if (bFoundClosingQuote)
		{
			CommandArray[i] = Combined;

			// Remove consumed entries
			const int32 NumToRemove = EndIndex - i;
			CommandArray.RemoveAt(i + 1, NumToRemove, true);
		}
	}
}

void ATerminalApplication::ParseCommands(TArray<FString>& CommandArray, FTerminalCommandExecutionParameters& CommandParameters)
{
	/*parse out commands, subcommands, and flags*/
	for (int32 i = 0; i < CommandArray.Num(); i++)
	{
		if (i == 0)
		{
			CommandParameters.Command = FName(CommandArray[0]);
			continue;
		}

		if (i == 1 && !CommandArray[1].StartsWith("-"))
		{
			FName Cmd = FName(CommandArray[1]);
			CommandParameters.Subcommands.Add(FTerminalSubcommand(Cmd));
		}

		/*handle flags*/
		if (i > 0)
		{
			/*grab command and surrounding ones*/
			FString Cmd = CommandArray[i];
			FString PrevCmd = CommandArray[i - 1];
			FString NextCmd = (i + 1) < CommandArray.Num() ? CommandArray[i + 1] : "";

			if (Cmd.Left(1) == "-") //check to see if this is marked as a flag
			{
				/*determines if this flag has a value attached to it*/
				if (NextCmd.IsEmpty() || NextCmd.Left(1) == "-")
					CommandParameters.Flags.Add(FTerminalCommandFlag(Cmd)); //add flag (no value)
				else if(!NextCmd.IsEmpty() && NextCmd.Left(1) != "-")
					CommandParameters.Flags.Add(FTerminalCommandFlag(Cmd, NextCmd)); //add flag with value
			}
		}
	}	
}

void ATerminalApplication::AddCommandToHistory(FString Command)
{
	CommandHistory.Insert(Command, 0); //always insert the latest command into the front of the array
}

void ATerminalApplication::ResetCommandHistory()
{
	CommandHistoryIndex = -1;
}

FString ATerminalApplication::GetPreviousCommand()
{
	if (CommandHistory.Num() > 0)
	{
		/*check to see if there's more history*/
		if (CommandHistoryIndex + 1 < CommandHistory.Num())
			CommandHistoryIndex++; //increment the history to the last entry
		
		/*return history entry*/
		return CommandHistory[CommandHistoryIndex];
	}
	else
		return FString(); //returns empty string
	
}

FString ATerminalApplication::GetNextCommand()
{
	if (CommandHistory.Num() > 0)
	{
		/*return a command if it exists*/
		if (CommandHistoryIndex - 1 > -1)
			CommandHistoryIndex--;
		
		/*clear command history selection and return an empty string for the UI to process*/
		else
		{
			
			ResetCommandHistory();
			return FString();
		}			

		return CommandHistory[CommandHistoryIndex];
	}

	return FString(); //returns empty string
}

void ATerminalApplication::ClearCommandHistory()
{
	CommandHistory.Empty();
}

bool ATerminalApplication::BlockedCommand(const FString& Command)
{
	TArray<FString> BlockedCommands = {
		TEXT("mkdir"),
		TEXT("rmdir"),
		TEXT("chmod"),
		TEXT("systemctl"),
		TEXT("mv"),
		TEXT("nano"),
		TEXT("vi"),
		TEXT("kill"),
		TEXT("killall"),
		TEXT("ifconfig"),
		TEXT("chown"),
		TEXT("bash"),
		TEXT("sh")
	};

	return BlockedCommands.Contains(Command);
}


