
#include "OperatingSystem/Applications/Commands/TerminalCommand_qrscanner.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_qrscanner::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{

    if (Terminal) {
        bool fileExists = true;
        AOperatingSystem* OS = Terminal->GetOperatingSystem();
        FString currentUser = OS->GetCurrentUser();
        FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
        FString HomeDirectory = OS->GetHomeDirectory();

        bool ToolExists = Terminal->CheckToolExists("qrscanner");
        if (!ToolExists) {
            Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, "qrscanner");
            return FTerminalCommandResult();
        }
        
        bool flagExport = false;
        bool flagAuthUser = false;
        bool flagListDevices = false;
        bool flagDevice = false;
        bool fileExtension = false;
        bool fourthWall = false;
        FString commandExport;
        if (CommandParameters.Flags.Num() < 1) {
            Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
            Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
            return FTerminalCommandResult();
        }

        for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
        {
            if (Flag.Flag == "help")
            {
                Terminal->PrintToTerminal(getHelpText());
                return FTerminalCommandResult();
            }
            if (Flag.Flag == "auth-user")
            {
                if (Flag.Value == currentUser.ToLower())
                    flagAuthUser = true;

            }
            if (Flag.Flag == "export")
            {
                commandExport = Flag.Value;
                fileExists = OS->FileSystemCheckIfFileExists(commandExport, ActiveWorkingDirectory, FOperatingSystemFileType::File);
                flagExport = true;
            }
            if (Flag.Flag == "device")
            {
                flagDevice = true;
            }
            if (Flag.Flag == "list-devices")
            {
                flagListDevices = true;
            }
            if (Flag.Flag == "fourthwall")
            {
                fourthWall = true;
            }
        }

        if (flagListDevices) {
            Terminal->PrintToTerminal(listDevices());
            return FTerminalCommandResult();
        }

        Terminal->PrintToTerminal("Checking user authorizatiohn...", ETerminalMessageStyle::Status);
        if (!flagAuthUser) {
            Terminal->PrintToTerminal("Invalid or missing authorized user: know who you are",ETerminalMessageStyle::Error, 1.5);
            return FTerminalCommandResult();
        }
        else {
            Terminal->PrintToTerminal("Authorized user accepted", ETerminalMessageStyle::OK,1.5);
        }

        if (!flagDevice) {
            Terminal->PrintToTerminal("No camera device ID defined.", ETerminalMessageStyle::Error);
            return FTerminalCommandResult();
        }

        if (flagDevice) 
        {
            FString DeviceID = GetFlagValue("device",CommandParameters);
            Terminal->PrintToTerminal("Checking device validity...", ETerminalMessageStyle::Status);
            if (DeviceID.IsEmpty())
            {
                Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
                return FTerminalCommandResult();
            }

            Terminal->PrintToTerminal("Initiating deviceID[" + DeviceID + "]...", ETerminalMessageStyle::Status, 0.5);

            if (DeviceID != "5")
            {
                Terminal->PrintToTerminal("Unable to initiate deviceID[" + DeviceID + "]", ETerminalMessageStyle::Error, 0.5);
                return FTerminalCommandResult();
            }

            else {
                Terminal->PrintToTerminal("Device successfully initiated", ETerminalMessageStyle::OK, 0.5);
                //TODO: This is where the program/game should stop in terminal so user can scan their arm/show animation; currently delayed for 3 seconds to mock scanning
                Terminal->PrintToTerminal("Waiting for QR code to scan ...", ETerminalMessageStyle::Status);
                //TODO: When the scan completes, proceed to next line of code
                Terminal->PrintToTerminal("QR code detected", ETerminalMessageStyle::OK, 3.5);
                Terminal->PrintToTerminal("Processing data...", ETerminalMessageStyle::Status, 1.5);
                Terminal->PrintToTerminal("QR decoded", ETerminalMessageStyle::OK, 0.5);
                Terminal->PrintToTerminal("User key detected in QR data", ETerminalMessageStyle::OK, 0.5);
                Terminal->PrintToTerminal("Preparing key for display", ETerminalMessageStyle::Status);
                Terminal->PrintToTerminal(privateKeyShow(), ETerminalMessageStyle::None, 2.5);
            }
        }

        if (flagExport) 
        {
            FString FileName = GetFlagValue("export", CommandParameters);

            if (FileName.Contains("/"))
            {
                Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxFormat, FileName);
                return FTerminalCommandResult();
            }

            else 
            {
                Terminal->PrintToTerminal("Processing file export request ... ", ETerminalMessageStyle::Status);
                if (fileExists) 
                {
                    Terminal->PrintToTerminal("Unable to export.  File " + FileName + " already exists.", ETerminalMessageStyle::Error, 0.5);
                    goto CheckFourth;
                }
                
                Terminal->PrintToTerminal("Exporting to file " + FileName, ETerminalMessageStyle::Status, 0.5);
                
                if (OS) 
                {
                    //TODO: Need to check real PGP key file size in linux and permissions
                    //TODO: need to check if exists as not to duplicate 
                    OS->FileSystemAddFile((FileName + ".asc"), FDateTime::Now(), "/Keys", 436, "rx-xx-rf", FOperatingSystemFileType::File, false, "PlayerPrivateKey");
                }
                Terminal->PrintToTerminal("Successfully exported private key to ~/" + HomeDirectory + "/Keys/" + FileName + ".asc", ETerminalMessageStyle::OK, 1.5);
            }
            
        }

    CheckFourth:
        if (fourthWall) 
        {
            //TODO: Need to update this message to make sense for ARG quest; this is placeholder
            //TODO: Also have the ability to grab more about their system to "freak the user out" as it shows more about them (card entered in Trello)
            //TODO: Need to consider animation here that freaks player out as breaking 4th wall (blinking desktop, showing system info, character in game reacts to it, etc..)
            Terminal->PrintToTerminal("Requested to break the fourth wall ... ", ETerminalMessageStyle::Status);
            FString RealUsername = OS->GetRealWorldUsername();
            FString KeyContent = privateKeyShow();
            Terminal->PrintToTerminal("O_o ... are you ready, <User>" + RealUsername + "</>?", ETerminalMessageStyle::Status, 3.0);
            //TODO: onion is duck duck go for now; key is fake; need to make this real
            FString ARGExport = OS->ExportARGFile("thebreadcrumb", KeyContent + "\n\nIt's all a lie: duckduckgogg42xjoc72x3sjasowoarfbgcmvfimaftt6twagswzczad.onion");
            Terminal->PrintToTerminal("You opened a door that can't be closed... your system has been tapped...", ETerminalMessageStyle::None);
            Terminal->PrintToTerminal("Breadcrumb has been dropped at: " + ARGExport, ETerminalMessageStyle::OK, 1.5);
        }

    }
    return FTerminalCommandResult();

}

FString UTerminalCommand_qrscanner::getHelpText() const {
    return TEXT(R"HELP(
NAME
    qrscanner :: command line terminal (CLI) qr code scanner

SYNOPSIS
    qrscanner [OPTIONS]

DESCRIPTION
    command line terminal (CLI) based qr code scanner
    enables low-resolution camera scanning of qr codes

OPTIONS
    --help
        Displays qrscanner help file (this text)

    --list-devices
        Shows a list of connected and compatible cameras and [deviceID] value

    --auth-user [Authorized User]
        Authorized user ID is required; know who you are

    --device [deviceID]
        The [deviceID] to scan code with; must be a compatible camera

    --export [file]
        Exports QR data to a specified file; required to use key with other tools

    --fourthwall
        O_o ...

EXAMPLE

    qrscanner --auth-user abc123 --device 2 --export examplefilename

)HELP");
}

FString UTerminalCommand_qrscanner::listDevices() const {
    return TEXT(R"HELP(
+----+-------------------------------+----------------------+------------------------------+---------------------------+
| ID | Name | Type | Device Path(s) | Bus Info |
+----+-------------------------------+----------------------+------------------------------+---------------------------+
| 0 | USB Optical Mouse | Input (HID) | /dev/input/mouse0 | usb-0000:00:14.0-1 |
| 1 | USB Mechanical Keyboard | Input (HID) | /dev/input/event3 | usb-0000:00:14.0-2 |
| 2 | Realtek HD Audio | Audio (ALSA) | /dev/snd/pcmC0D0p | pci-0000:00:1f.3 |
| 3 | Kingston DataTraveler 3.0 | Storage (Mass) | /dev/sdb | usb-0000:00:14.0-6 |
| 4 | Intel Ethernet Controller | Network (Ethernet) | /dev/net/tun | pci-0000:00:19.0 |
| 5 | Logitech HD Pro Webcam C920 | Video (USB Camera) | /dev/video0, /dev/video1 | usb-0000:00:14.0-3 |
| 6 | NVIDIA GPU | Graphics (PCIe) | /dev/dri/card0 | pci-0000:01:00.0 |
| 7 | USB Wireless Adapter | Network (WiFi) | /dev/wlan0 | usb-0000:00:14.0-7 |
+----+-------------------------------+----------------------+------------------------------+---------------------------+
)HELP");
}

//TODO: Need to get a real key in here eventually
FString UTerminalCommand_qrscanner::privateKeyShow() const {
    return TEXT(R"HELP(
-----BEGIN PRIVATE KEY BLOCK-----
mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/U
b7O1u120JkFsaWNlIExvdmVsYWNlIDxhbGljZUBvcGVucGdwLmV4YW1wbGU+iJAE
ExYIADgCGwMFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AWIQTrhbtfozp14V6UTmPy
MVUMT0fjjgUCXaWfOgAKCRDyMVUMT0fjjukrAPoDnHBSogOmsHOsd9qGsiZpgRnO
dypvbm+QtXZqth9rvwD9HcDC0tC+PHAsO7OTh1S1TC9RiJsvawAfCPaQZoed8gK4
OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3s
E9+eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIb
DAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/cDW4xN80fsn
0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE=
=iIGO
-----END PRIVATE KEY BLOCK-----
)HELP");
}

UTerminalCommand_qrscanner::UTerminalCommand_qrscanner()
{
    Command = "qrscanner";
}
