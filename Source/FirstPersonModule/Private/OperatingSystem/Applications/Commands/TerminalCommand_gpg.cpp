// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_gpg.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_gpg::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{

	AOperatingSystem* OS = Terminal->GetOperatingSystem();
	FString ActiveWorkingDirectory = OS->GetActiveWorkingDirectory();
	FString HomeDirectory = OS->GetHomeDirectory();
	bool DetachSign = false;
	bool VerifySignature = false;

	TArray<FOperatingSystemFiles> FileResult;

	bool ToolExists = Terminal->CheckToolExists("gpg");
	if (!ToolExists) {
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, "gpg");
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::UseHelp);
		return FTerminalCommandResult();
	}

	if (CommandParameters.Flags.Num() < 1) {
		Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength);
		return FTerminalCommandResult();
	}

	for (FTerminalCommandFlag& Flag : CommandParameters.Flags)
	{
		if (Flag.Flag == "help")
		{
			Terminal->PrintToTerminal(getHelpText());
			return FTerminalCommandResult();
		}

		if (Flag.Flag == "about")
		{
			Terminal->PrintToTerminal(getAboutText());
			return FTerminalCommandResult();
		}

		if (Flag.Flag == "keys")
		{
			FileResult = OS->FileSystemListFiles();
			for (int32 i = 0; i < FileResult.Num(); i++) {
				if (FileResult[i].Path == "/Keys") {
					if (FileResult[i].UDF1 == "Lambda5PublicKey") {
						Terminal->PrintToTerminal("<OK>[AUTHORIZED PUBLIC KEY]</> " + FileResult[i].Name);
						continue;
					}
					if (FileResult[i].UDF1 == "PlayerPrivateKey") {
						Terminal->PrintToTerminal("<OK>[AUTHORIZED PRIVATE KEY]</> " + FileResult[i].Name);
						continue;
					}
					if (FileResult[i].Name.EndsWith(".asc")) {
						Terminal->PrintToTerminal("<ERROR>[UNAUTHORIZED KEY]</> " + FileResult[i].Name);
						continue;
					}
					else
					{
						Terminal->PrintToTerminal("[UNKNOWN FILE (NON-KEY)] " + FileResult[i].Name);
						continue;
					}
				}
				
			}
			return FTerminalCommandResult();
		}

		if (Flag.Flag == "list-signatures")
		{
			FileResult = OS->FileSystemListFiles();
			FString PathPrefix;
			for (int32 i = 0; i < FileResult.Num(); i++) {
				if (FileResult[i].Name.EndsWith(".sig")) {
					PathPrefix = PathPrefix = "~/" + HomeDirectory;
					if (FileResult[i].Path == "") { PathPrefix = PathPrefix + "/"; }
					else {
						PathPrefix = PathPrefix + FileResult[i].Path + "/";
					}
					Terminal->PrintToTerminal("<STATUS>[SIGNATURE FILE]</> " + PathPrefix + FileResult[i].Name);
				}

			}
			return FTerminalCommandResult();
		}

		if (Flag.Flag == "detach-sign") {
			DetachSign = true;
		}

		if (Flag.Flag == "verify") {
			VerifySignature = true;
		}

		if (DetachSign) {
			FString DirectoryPath = ActiveWorkingDirectory + "/";
			FString FileToSign = GetFlagValue("detach-sign", CommandParameters);

			if (FileToSign.IsEmpty()) {
				Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength, ":: missing file to sign with --detach-sign");
				return FTerminalCommandResult();
			}

			Terminal->PrintToTerminal("Preparing signature of file: ~/" + HomeDirectory + DirectoryPath + FileToSign);
			bool CheckFileExists = OS->FileSystemCheckIfFileExists(FileToSign,ActiveWorkingDirectory,FOperatingSystemFileType::File);
			if (!CheckFileExists) {
				Terminal->PrintToTerminal("Unable to sign: File does not exist: ~/" + HomeDirectory + DirectoryPath + FileToSign, ETerminalMessageStyle::Error, 0.5);
				return FTerminalCommandResult();
			}
			FOperatingSystemFiles PrivateKeyFile = OS->FileSystemGetFile("", "", "PlayerPrivateKey");
			if (PrivateKeyFile.Name.IsEmpty()) {
				Terminal->PrintToTerminal("Unable to sign: No private key detected on system", ETerminalMessageStyle::Error, 0.5);
				return FTerminalCommandResult();
			}
			Terminal->PrintToTerminal("User private key loaded: ~/" + HomeDirectory + PrivateKeyFile.Path + "/" + PrivateKeyFile.Name, ETerminalMessageStyle::OK, 0.5);
			OS->FileSystemAddFile(FileToSign + ".sig", FDateTime::Now(), ActiveWorkingDirectory, 0, "rx-xx-rf", FOperatingSystemFileType::File, false, "AuthenticSignature:Player");
			Terminal->PrintToTerminal("Signature file generated: ~/" + HomeDirectory + DirectoryPath + FileToSign + ".sig", ETerminalMessageStyle::OK, 0.5);
			return FTerminalCommandResult();
		}

		if (VerifySignature) {
			FString FileToVerify = GetFlagValue("verify", CommandParameters);

			if (FileToVerify.IsEmpty()) {
				Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::SyntaxLength, ":: missing filename to verify with --verify");
				return FTerminalCommandResult();
			}

			bool CheckFileExists = OS->FileSystemCheckIfFileExists(FileToVerify, ActiveWorkingDirectory, FOperatingSystemFileType::File);

			if (!CheckFileExists) {
				Terminal->PrintToTerminal("Unable to verify: File does not exist: " + FileToVerify, ETerminalMessageStyle::Error, 0.5);
				return FTerminalCommandResult();
			}

			bool FoundSignatureFile = false;
			bool AuthenticSignatureFile = false;
			FString FileSigName = FileToVerify + ".sig";
			FileResult = OS->FileSystemListFiles();
			for (int32 i = 0; i < FileResult.Num(); i++) {
				if (FileResult[i].Name == FileSigName) {
					FoundSignatureFile = true;
					if (FileResult[i].UDF1 == "AuthenticSignature:Player" || FileResult[i].UDF1 == "AuthenticSignature:L5") {
						AuthenticSignatureFile = true;
					}
				}
			}

			if (!FoundSignatureFile) {
				Terminal->PrintToTerminal("Unable to verify: No signature (.SIG) exists for: " + FileToVerify, ETerminalMessageStyle::Error, 0.5);
				return FTerminalCommandResult();
			}

			if (FoundSignatureFile && !AuthenticSignatureFile) {
				Terminal->PrintToTerminal("<ERROR>[WARNING]</> Signature (" + FileSigName + ") is not able to be verified for " + FileToVerify, ETerminalMessageStyle::None, 0.5);
				Terminal->PrintToTerminal("<ERROR>[WARNING]</> System may be compromised; shutdown immediately to clear data", ETerminalMessageStyle::None, 0.5);
				return FTerminalCommandResult();
			}

			if (FoundSignatureFile && AuthenticSignatureFile) {
				Terminal->PrintToTerminal("<OK>[SIGNATURE VERIFIED]</> Signature (" + FileSigName + ") verified against file " + FileToVerify, ETerminalMessageStyle::None, 0.5);
				return FTerminalCommandResult();
			}
		}
	}
	return FTerminalCommandResult();
}

FString UTerminalCommand_gpg::getHelpText() const {
	return TEXT(R"HELP(
NAME
    gpg :: a GnuPG (Privacy Guard) tool modified for S.Light protocol encryption

SYNOPSIS
    gpg [OPTIONS]

DESCRIPTION
    command line terminal (CLI) GPG tool designed to work with S.Light protocol encryption
	tool aides in encrypton, decryption, crytographic signing, and crytographic signature validation
	NOTE: this is a heavily modified GnuPG tool intended to streamline organizational use

	learn more about the encryption by typing "help --about"

OPTIONS
    --help
        displays qrscanner help file (this text)

	--about
		overview of S.Light protocol and encryption orientation

    --keys
        scans and lists keys (.asc) located in the system KEYS directory

	--list-signatures
		scans for and lists signature files (.sig) located in the system
		useful to see available signature files by name and location

	--detach-sign [filename]
		signs defined file using default private key in KEYS directory
		creates seperate filename.sig file without changing file signed
		NOTE: command must be run in directory of file being signed

	--verify [file to verify]
		verifies signature file in folder against signed source
		NOTE:
			command must be run in directory of file being validated
			signatures (.sig) and file must be in same folder to run
		
EXAMPLES

    gpg --detach-sign asamplefile
	gpg --verify asamplefile

)HELP");
}

FString UTerminalCommand_gpg::getAboutText() const {
	return TEXT(R"HELP(
NAME
    gpg :: a GnuPG (Privacy Guard) tool modified for S.Light protocol encryption

OVERVIEW:
	The success of the mission depends on secure communication.
	The threat you are escaping can observe all unprotected channels.
	Unsecured transmissions risk immediate compromise.

	All outbound communications must be encrypted before transmission.
	All transmitted files must be signed to confirm origin.
	All received files must be verified. Trust nothing by default.

KEYS:
	Keys establish identity and trust between us.
	Each of us holds a matched key pair (public and private).

	The public key is shared openly and used to encrypt data.
	The private key is kept secret and used to decrypt data.

	Anything encrypted with your public key can only be read
	with your private key.  Anything encrypted with our public
	key can only be read with our private key.

	If your private key is exposed, your identity is compromised.
	Protect it at all times.

SIGNATURES:
	Signatures are used as proof a file came from a trusted source.
	A signature is stored as a separate .sig file.

	The sender signs the file using their private key.
	The signing of a file generates a .SIG file to verify.
	The receiver verifies it using the senders public key.


	If verification fails, the file may be altered or forged.
	Never trust or execute unverified files.
)HELP");
}

UTerminalCommand_gpg::UTerminalCommand_gpg()
{
    Command = "gpg";
}
