
#include "OperatingSystem/Applications/Commands/TerminalCommand_qrscanner.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_qrscanner::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{

    if (Terminal) {
        //TODO: Research: Any scenario where currentUser or OS fails to grab?
        bool fileExists = true;
        AOperatingSystem* OS = Terminal->GetOperatingSystem();
        FString currentUser = OS->GetCurrentUser();
        FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
        FString HomeDirectory = OS->GetHomeDirectory();

        //Checks if player deleted their tool
        FOperatingSystemFiles FileResult;

        FileResult = OS->FileSystemGetFile("", "", "Tool:qrscanner");

        if (FileResult.Name == "") {
            Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, "qrscanner");
            return FTerminalCommandResult();
        }

        //TODO: These flags use : parse versus "command" after flag; true linx would be --auth-key username_here where current design is --auth-key:username_here
        bool flagExport = false;
        bool flagAuthKey = false;
        bool flagListDevices = false;
        bool flagDevice = false;
        bool fileExtension = false;
        bool fourthWall = false;
        TArray<FString> commandExportSplit;
        FString commandExport;
        TArray<FString> commandAuthKeySplit;
        TArray<FString> commandDeviceSplit;
        if (CommandParameters.Flags.Num() < 1) {
            Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
            return FTerminalCommandResult();
        }
        for (int32 i = 0; i < CommandParameters.Flags.Num(); i++) {
            FString currentParameter = CommandParameters.Flags[i];
            if (currentParameter == "--help") {
                Terminal->PrintToTerminal(getHelpText());
                return FTerminalCommandResult();
            }
            if (currentParameter.Left(11) == "--auth-user") {
                currentParameter.ParseIntoArray(commandAuthKeySplit, TEXT(":"), true);
                if (commandAuthKeySplit[1].Equals(currentUser, ESearchCase::CaseSensitive)) {
                    flagAuthKey = true;
                }
            }
            if (currentParameter.Left(8) == "--export") {
                commandExport = currentParameter;
                currentParameter.ParseIntoArray(commandExportSplit, TEXT(":"), true);
                fileExists = OS->FileSystemCheckIfFileExists(commandExportSplit[1], ActiveWorkingDirectory, FOperatingSystemFileType::File);
                flagExport = true;
            }
            if (currentParameter == "--list-devices") {
                flagListDevices = true;
            }
            if (currentParameter.Left(8) == "--device") {
                currentParameter.ParseIntoArray(commandDeviceSplit, TEXT(":"), true);
                flagDevice = true;
            }
            if (currentParameter.Left(12) == "--fourthwall") {
                fourthWall = true;
            }
        }

        // TODO: Need to bake in delays/loading animations into this function
        if (flagListDevices) {
            Terminal->PrintToTerminal(listDevices());
            return FTerminalCommandResult();
        }

        if (!flagAuthKey) {
            Terminal->PrintToTerminal("Invalid or missing authorized user key: know who you are",ETerminalMessageStyle::Error);
            return FTerminalCommandResult();
        }
        else {
            Terminal->PrintToTerminal("Authorized user key accepted", ETerminalMessageStyle::OK);
        }

        if (flagDevice) {
            if (commandDeviceSplit.Num() != 2) {
                Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
                return FTerminalCommandResult();
            }
            Terminal->PrintToTerminal("Initiating deviceID[" + commandDeviceSplit[1] + "]...", ETerminalMessageStyle::Status);
            if (commandDeviceSplit[1] != "5") {
                Terminal->PrintToTerminal("Unable to initiate deviceID[" + commandDeviceSplit[1] + "]", ETerminalMessageStyle::Error);
                return FTerminalCommandResult();
            }
            else {
                Terminal->PrintToTerminal("Device successfully initiated", ETerminalMessageStyle::OK);
                Terminal->PrintToTerminal("Waiting for QR code to scan ...", ETerminalMessageStyle::Status);
                //TODO: This is where the program/game should stop in terminal so user can scan their arm / show animation
                //TODO: When the scan completes, proceed to next line of code
                Terminal->PrintToTerminal("QR code detected", ETerminalMessageStyle::OK);
                Terminal->PrintToTerminal("Processing QR data...", ETerminalMessageStyle::Status);
                Terminal->PrintToTerminal("QR decoded", ETerminalMessageStyle::OK);
                //TODO: Want to have an image load here to shows a QR code in the terminal screen with meta data under it (I can do terminal prints for meta data ; just need a way to load an image)
                Terminal->PrintToTerminal("User key detected in QR data", ETerminalMessageStyle::OK);
                Terminal->PrintToTerminal(privateKeyShow());
            }
        }

        if (!flagDevice) {
            Terminal->PrintToTerminal("No camera defined.", ETerminalMessageStyle::Error);
            return FTerminalCommandResult();
        }

        if (flagExport) {
            if (commandExportSplit[1].Contains("/") || commandExportSplit.Num() != 2) {
                Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxFormat, commandExport);
                return FTerminalCommandResult();
            }
            else {
                Terminal->PrintToTerminal("Processing file export request ... ", ETerminalMessageStyle::Status);
                if (fileExists) {
                    Terminal->PrintToTerminal("Unable to export.  File " + commandExportSplit[1] + " already exists.", ETerminalMessageStyle::Error);
                    goto CheckFourth;
                }
                Terminal->PrintToTerminal("Exporting to file " + commandExportSplit[1], ETerminalMessageStyle::Status);
                if (OS) {
                    //TODO: Need to check real PGP key file size in linux and permissions
                    OS->FileSystemAddFile((commandExportSplit[1] + ".asc"), FDateTime::Now(), "/Keys", 436, "rx-xx-rf", FOperatingSystemFileType::File, false, "PlayerPrivateKey");
                }
                Terminal->PrintToTerminal("Successfully exported private key to ~/" + HomeDirectory + "/Keys/" + commandExportSplit[1] + ".asc", ETerminalMessageStyle::OK);
            }
            
        }

    CheckFourth:
        //TODO: Need a way to "write" this file to the system and record a global variable that this created file is the signature file (used outside of qrscanner); likely "UDF" array value to keep simple
        if (fourthWall) {
            //TODO: Need to update this message to make sense for ARG quest; this is placeholder
            //TODO: Also have the ability to grab more about their system to "freak the user out" as it shows more about them (card entered in Trello)
            Terminal->PrintToTerminal("Requested to break the fourth wall ... ", ETerminalMessageStyle::Status);
            FString RealUsername = OS->GetRealWorldUsername();
            FString KeyContent = privateKeyShow();
            Terminal->PrintToTerminal("O_o ... are you ready, " + RealUsername + "?", ETerminalMessageStyle::Status);
            //TODO: onion is duck duck go for now; key is fake; need to make this real
            FString ARGExport = OS->ExportARGFile("thebreadcrumb", KeyContent + "\n\nIt's all a lie: duckduckgogg42xjoc72x3sjasowoarfbgcmvfimaftt6twagswzczad.onion");
            Terminal->PrintToTerminal("Breadcrumb has been dropped at: " + ARGExport, ETerminalMessageStyle::OK);
        }

    }
    return FTerminalCommandResult();

}

FString UTerminalCommand_qrscanner::getHelpText() const {
    return TEXT(R"HELP(
NAME
    qrscanner - command line terminal (CLI) qr code scanner

SYNOPSIS
    qrscanner [OPTIONS]

DESCRIPTION
    command line terminal (CLI) based qr code scanner
    enables camera(s) for low reslution scanning of qr codes
    application requires sudo permissions to run

OPTIONS
    --help
        Displays qrscanner help file (this text)

    --list-devices
        Shows a list of connected and compatible cameras and [deviceID] value

    --auth-user:[Authorized User]
        Authorized user ID is required; know who you are

    --device:[deviceID]
        The [deviceID] to scan code with; must be a compatible camera

    --export:[file]
        Exports QR data to a specified file; required to use key with other tools

    --fourthwall
        O_o ...

EXAMPLE

    qrscanner --auth-user:abc123 --device:2 --export:myexportfile

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
