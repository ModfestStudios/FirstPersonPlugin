// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_Help.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"



FTerminalCommandResult UTerminalCommand_Help::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{

    if (CommandParameters.Flags.Num() > 1) {
        Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
        return FTerminalCommandResult();
    }

    for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
    {
        if (Flag == "commands")
        {
            Terminal->PrintToTerminal(getCommandsHelpText());
            return FTerminalCommandResult();
        }

        if (Flag == "about")
        {
            Terminal->PrintToTerminal(getAboutHelpText());
            return FTerminalCommandResult();
        }
    }

    //for (int32 i = 0; i < CommandParameters.Flags.Num(); i++) 
    //{
    //    FString currentParameter = CommandParameters.Flags[i];
    //    if (currentParameter == "--commands") {
    //        Terminal->PrintToTerminal(getCommandsHelpText());
    //        return FTerminalCommandResult();
    //    }
    //    if (currentParameter == "--about") {
    //        Terminal->PrintToTerminal(getAboutHelpText());
    //        return FTerminalCommandResult();
    //    }
    //}

    Terminal->ClearTerminal();
    Terminal->PrintToTerminal(getHelpText());
    return FTerminalCommandResult();
}

//TODO: Need to build out operating system help file structure/naming
//TODO: Need to come up with revision numbers / change of OS name as they progress
FString UTerminalCommand_Help::getHelpText() const {
    return TEXT(R"HELP(
w h i t e . r a b b i t s . O S [ h e l l o w o r l d ]

  _               __  __ ____  _____               _____ 
 | |        /\   |  \/  |  _ \|  __ \   /\        | ____|
 | |       /  \  | \  / | |_) | |  | | /  \ ______| |__  
 | |      / /\ \ | |\/| |  _ <| |  | |/ /\ \______|___ \ 
 | |____ / ____ \| |  | | |_) | |__| / ____ \      ___) |
 |______/_/    \_\_|  |_|____/|_____/_/    \_\    |____/ 


ORIENTATION
    it's all a lie.  not all is what it seems.
    trust the system.  trust yourself.
    you are being watched, we can't tell you everything.  
    we need information.  collect what we tell you to.
    
NEXT STEPS
    when you know who you are, scan your key into the system then type 'helloworld'
    Use 'help --commands' to learn how to navigate this system

)HELP");
}

FString UTerminalCommand_Help::getCommandsHelpText() const {
    return TEXT(R"HELP(
NAME
    help - operating system help / orientation file

SYNOPSIS
    help
        --commands
        --about

DESCRIPTION
    These shell commands are defined internally.

COMMANDS
    ls
        List files and directories in current directory

    clear
        Clear terminal screen

    echo [string]
        Echoes entered text back onto the terminal
        Caution: Can be disruptive to mission; only use when instructed
    
    whoami
        Display authorized user

    cd
        Change directory to navigate file system
        EXAMPLE: "cd Tools" to view tools folder

    touch
        Creates a file within the current directory

    rm
        Removes a file within the current directory

    helloworld
        A message just for you

TOOLS

    this system is preloaded with tools and commands crucial for your success
    commands can be found using "help --commands" to list
    all pre-loaded tools are within the tools directory
    to view available tools, view tool directory by typing 'cd tools'
    once inside the tools, type "ls" to see available tools
    type the tool name in the terminal and commit (enter) to run

ADDITIONAL HELP
    use "--help" flag with tools or commands for specific help details

)HELP");
}

FString UTerminalCommand_Help::getAboutHelpText() const {
    return TEXT(R"HELP(
BUILD AND VERSION
    helloworld 1.6
    GNU bash, version 5.2.21(1)-release (x86_64-pc-linux-gnu)
    Encryption layer: S.Light protocol

ABOUT
    a heavily customized GNU linux-based operating system     
    helloworld is a purpose built operating system for you
    it is your key to finding the answers your life depends on
    it is secure because it is amnesiac, rebooting wipes memory
    if you are at risk of compromise, type "shutdown now" to terminate

OPERATING SYSTEM OVERVIEW
    commands
        core functionality provided in the operating system
        type "help --commands" to see the list of commands
        commands are typed into the terminal (e.g. "help" is a command)

    flags
        additional functionality to use when executing a command/tool
        flags are denoted by a leading double-dash (--)
        for example "help --commands" the flag is "--commands"
        most tools have a "--help" flag for more tool details
        use the help flag to understand your tools

    directories and files
        for security purposes, helloworld is locked down to core files/directories
        using the "ls" command will list files and directories
        directories are shown in blue text; files in system default grey
        every default file and directory has a purpose, remember this

    s.light protocol 
        sensitive messages are s.light encrypted to avoid compromise
        s.light encryption is bleeding edge and m.verse quantum resistant
        all files maintain authentic signature (.sig) and key (in keys)
        NOTE: signature (.sig) files are hidden; use "ls -art" to see hidden files
        use this and keys folder to verify authenticity, compromise is a risk
        Note: all s.light protocols are embedded into the standard "gpg" tool
        NOTE: all returned drops must be encrypted to maintain OPSEC

AUTHOR
    written by Dr. Alto Clef
    architecture authored by Dr. Charles Gears
    crytographic protocol by Dr. Sophia Light

)HELP");
}

UTerminalCommand_Help::UTerminalCommand_Help()
{
	Command = "help";
}
