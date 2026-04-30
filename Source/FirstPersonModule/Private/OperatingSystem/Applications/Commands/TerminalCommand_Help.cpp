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
    when you know who you are, scan your key into the system then type "helloworld"
    Type "help --about" to learn more about this system
    Type "help --commands" to learn how to navigate this system

)HELP");
}

FString UTerminalCommand_Help::getCommandsHelpText() const {
    return TEXT(R"HELP(
NAME
    help :: operating system help / orientation file

SYNOPSIS
    help
    help --commands
    help --about

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

    This system is preloaded with tools crucial for your success.
    All pre-loaded tools are within the tools directory.
    To view available tools, access the tools directory by typing "cd tools"
    Once inside the tools directory, type "ls" to see available tools.
    Type the tool name in the terminal and commit (hit enter) to run

    EXAMPLE:
        qrscanner

    NOTE:
        Tools can be run from any directory.

ADDITIONAL HELP
    use "--help" flag with tools or commands for specific help details
    EXAMPLES:
        ls --help
        qrscanner --help

)HELP");
}

FString UTerminalCommand_Help::getAboutHelpText() const {
    return TEXT(R"HELP(
BUILD AND VERSION
    helloworld 1.6
    GNU bash, version 5.2.21(1)-release (x86_64-pc-linux-gnu)
    Encryption layer: S.Light protocol

ABOUT
    A heavily customized GNU linux-based operating system.     
    It is your key to finding the answers your life depends on.
    It is amnesiac; rebooting wipes memory, this is for your protection.
    If you are at risk of compromise, type "shutdown now" to terminate immediately.

OPERATING SYSTEM OVERVIEW
    commands
        commands are core functionality provided in the operating system
        type "help --commands" to see the list of available commands
        commands are typed into the terminal (e.g. "help" and "ls" are commands)
        NOTE: this OS is limited to key commands; non-critical are disabled/not listed

    flags
        additional functionality to use when executing a command/tool
        flags are denoted by a leading double-dash (--) or single dash (-)
        for example "help --commands" the flag is "--commands"
        most tools and commands have a "--help" flag for more tool details
        use the help flag for specific help on tools or commands
        EXAMPLES:
            ls --help
            qrscanner --help

    directories and files
        for security purposes, helloworld is locked down to mission files/directories
        you can only navigate to the folders and files important to your mission
        using the "ls" command will list the available files and directories
        directories are shown as <Directory>blue</> text; files are shown as grey
        every file and directory has a purpose, find it

    working directory prompt
        the prompt tells you which <directory>directory</> you are working in within the OS
            EXAMPLE1:
                <User>white-rabbit</>:<directory>~/lambda5</>$
                shows you are in the home directory <directory>lambda5</>
                NOTE: "~/" denotes "home" ; you cannot go any higher than home
            EXAMPLE2:
                <User>white-rabbit</>:<directory>~/lambda5/Tools</>$
                shows you are in the <Directory>Tools</> directory which is a child of home (<Directory>lambda5</>)
        to change directories, use the "cd" command

    s.light protocol 
        sensitive messages are s.light encrypted to avoid compromise
        s.light encryption is bleeding edge and m.verse quantum resistant
        all critical files maintain authentic signatures (.sig) and keys (in <Directory>Keys</>)
        NOTE: signature (.sig) files are hidden; use "ls -art" to see hidden files
        use this and keys folder to verify authenticity, compromising comms is a risk
        NOTE:
            all s.light protocols are embedded into the standard "gpg" tool
            all returned drops must be encrypted to maintain OPSEC
            use "gpg --help" for more details on encryption handling  

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
