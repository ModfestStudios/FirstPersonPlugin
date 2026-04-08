// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/OperatingSystem.h"
#include "UI/Widgets/OperatingSystemWidget.h"

/*terminal*/
#include "Interactives/Actors/TerminalSystem.h"

AOperatingSystem::AOperatingSystem()
{
	Users.Add(FOperatingSystemUser("admin", "password2"));

	bReplicates = true;
}

void AOperatingSystem::InitializeFromTerminal(ATerminalSystem* Terminal)
{
	TerminalSystem = Terminal;
	
	if (Terminal)
	{
		Widget = Terminal->GetOperatingSystemWidget();

		if (Widget)
			Widget->OperatingSystem = this;
	}
}


//==============
//=====BOOT=====
//==============
void AOperatingSystem::Boot()
{	
	if (SystemState == EOperatingSystemMainState::PoweredOff)
		BeginBootSequence();
}

void AOperatingSystem::BeginBootSequence()
{
	/*only want to boot if it's from a power-off state*/
	if (SystemState == EOperatingSystemMainState::PoweredOff)
	{
		SystemState = EOperatingSystemMainState::POST; //go into the Power-On Self Test first
		BeginPOST();
	}
}

void AOperatingSystem::InitiateBIOS()
{

}

void AOperatingSystem::BeginPOST()
{

}

void AOperatingSystem::EndBootSequence()
{

}

FString AOperatingSystem::GetActiveWorkingDirectory() const
{
	return FString(ActiveWorkingDirectory);
}

FString AOperatingSystem::GetHomeDirectory() const
{
	return FString(HomeDirectory);
}

void AOperatingSystem::SetActiveWorkingDirectory(const FString& NewWorkingDirectory)
{
	ActiveWorkingDirectory = NewWorkingDirectory;
	return;
}

FString AOperatingSystem::ExportARGFile(const FString& Name, const FString& FileContent)
{
	FString UserDir = FPlatformProcess::UserDir();
	FString FullPath = UserDir / (Name + TEXT(".txt"));

	bool bSuccess = FFileHelper::SaveStringToFile(FileContent, *FullPath);

	return FullPath;
}

FString AOperatingSystem::GetRealWorldUsername()
{
	FString ReturnedUser = FPlatformProcess::UserName();
	return ReturnedUser;
}

EOperatingSystemMainState AOperatingSystem::GetSystemState()
{
	return SystemState;
}

/* TODO: Old directory structure
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
*/

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

//TODO: Potentially need to do a return of true/flase for success or failure; right now logic just assumes it always works.  Not sure if there's a scenario where it "doesn't work" since it's just an array
void AOperatingSystem::FileSystemAddFile(const FString& Name, const FDateTime& Date, const FString& Directory, const int32& Size, const FString& Permissions, FOperatingSystemFileType Type, bool bIsHidden, FString UDF1)
{
	SystemFiles.Add(FOperatingSystemFiles(Name, Date, Directory, Size, Permissions, Type, bIsHidden, UDF1));
}

void AOperatingSystem::FileSystemDeleteFile(const FString& Name, const FString& Path, FOperatingSystemFileType Type)
{
	for (int32 i = SystemFiles.Num() - 1; i >= 0; i--)
	{
		if (SystemFiles[i].Name == Name &&
			SystemFiles[i].Path == Path &&
			SystemFiles[i].Type == Type)
		{
			SystemFiles.RemoveAt(i);
		}
	}
}

bool AOperatingSystem::FileSystemCheckIfFileExists(const FString& Name, const FString& Path, FOperatingSystemFileType Type)
{
	bool checkFileResult = false;
	FString NameClean = Name;


	if (Name.StartsWith(".")) {
		NameClean = Name.RightChop(1);
	}

	for (int32 i = SystemFiles.Num() - 1; i >= 0; i--)
	{
		if (SystemFiles[i].Name == NameClean &&
			SystemFiles[i].Path == Path &&
			SystemFiles[i].Type == Type)
		{
			checkFileResult = true;
		}
	}

	return checkFileResult;

}

TArray<FOperatingSystemFiles> AOperatingSystem::FileSystemListFiles(FOperatingSystemFileSortBy SortBy) const
{
	TArray < FOperatingSystemFiles> SortedFiles = SystemFiles;
	switch (SortBy)
	{
	case FOperatingSystemFileSortBy::Name:
		SortedFiles.Sort([](const FOperatingSystemFiles& A, const FOperatingSystemFiles& B)
			{
				return A.Name < B.Name;
			});
		break;
		//TODO: Need to test this when file sizing is implemented properly; also need LS command to support sort by size formatting
	case FOperatingSystemFileSortBy::Size:
		SortedFiles.Sort([](const FOperatingSystemFiles& A, const FOperatingSystemFiles& B)
			{
				return A.Size < B.Size;
			});
		break;
	case FOperatingSystemFileSortBy::Date:
			SortedFiles.Sort([](const FOperatingSystemFiles& A, const FOperatingSystemFiles& B)
				{
					return A.Date < B.Date;
				});
			break;
	}


	return SortedFiles;
}

// Search for specific file by name, path, udf1 (returns first one found; will not sort results)
FOperatingSystemFiles AOperatingSystem::FileSystemGetFile(const FString& Name, FString Path, FString UDF1)
{
	for (const FOperatingSystemFiles& File : SystemFiles)
	{
		const bool bNameMatches = Name.IsEmpty() || File.Name == Name;
		const bool bPathMatches = Name.IsEmpty() || File.Path == Path;
		const bool bUDF1Matches = UDF1.IsEmpty() || File.UDF1 == UDF1;

		if (bNameMatches && bPathMatches && bUDF1Matches)
		{
			return File;
		}
	}

	return FOperatingSystemFiles();
}


void AOperatingSystem::BeginPlay()
{
	Super::BeginPlay();

	FMath::RandInit(FDateTime::Now().GetTicks());

	//TODO: For now this is just the first OS experience call; intend is to change this to be a trigger based on "phase" of game so the OS can evolve from a structure/tools perspective
	FileSystemAddFile("Keys", FDateTime(2026, 4, 3), "", 0, "-rwxr-xr--", FOperatingSystemFileType::Directory, false);
	FileSystemAddFile("Tools", FDateTime(2026, 4, 3), "", 34589, "-rwxr-xr--", FOperatingSystemFileType::Directory, false);
	FileSystemAddFile("helloworld", FDateTime(2026, 4, 3), "", 11420, "-r--r--r--", FOperatingSystemFileType::File, false, "Tool:helloworld");
	FileSystemAddFile("helloworld.sig", FDateTime(2026, 4, 3), "", 232, "-r--r--r--", FOperatingSystemFileType::File, true);
	FileSystemAddFile("winningsequence", FDateTime(2026, 4, 3), "", 0, "-rwxr-xr--", FOperatingSystemFileType::Directory, true);
	GenerateLotteryFiles();
	//TODO: Winning Sequence identified file
	//The game can randomize ths number (if needed) and place this in, the tag at the end is what determines it is in fact the right winning sequence number
	FileSystemAddFile("07-19-32-43-51-14", FDateTime(2019, 3, 25), "/winningsequence", 0, "-r--r--r--", FOperatingSystemFileType::File, false,"DIG-004:winningsequence");

	//DEBUG: Used for testing children of children directories and files
	//FileSystemAddFile("Test", FDateTime(2026, 4, 3), "/Tools", 34589, "-rwxr-xr--", FOperatingSystemFileType::Directory, false);
	//FileSystemAddFile("Test2", FDateTime(2026, 4, 3), "/Tools/Test", 34589, "-rwxr-xr--", FOperatingSystemFileType::Directory, false);

	//TODO: Need to determine naming for key; are we going with rabbits or another, etc.. 
	FileSystemAddFile("lambda5_public.asc", FDateTime(2026, 4, 3), "/Keys", 436, "-rwxr-xr--", FOperatingSystemFileType::File, false, "Lambda5PublicKey");
	FileSystemAddFile("qrscanner", FDateTime(2026, 4, 3), "/Tools", 22237, "-rwxr-xr--", FOperatingSystemFileType::File, false, "Tool:qrscanner");
	FileSystemAddFile("qrscanner.sig", FDateTime(2026, 4, 3), "/Tools", 232, "-r--r--r--", FOperatingSystemFileType::File, true);
	FileSystemAddFile("gpg", FDateTime(2026, 4, 3), "/Tools", 12352, "-rwxr-xr--", FOperatingSystemFileType::File, false, "Tool:gpg");
	FileSystemAddFile("gpg.sig", FDateTime(2026, 4, 3), "/Tools", 232, "-r--r--r--", FOperatingSystemFileType::File, true);

	FString KeypadFilePath = "/openthenoor";
	FileSystemAddFile(KeypadFilePath.RightChop(1), FDateTime(2009, 3, 14), "", 76175, "-rwxr-xr--", FOperatingSystemFileType::Directory, true);
	FileSystemAddFile("K7-17736", FDateTime(2009, 3, 14), KeypadFilePath, 142, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("E3-21182", FDateTime(2011, 7, 22), KeypadFilePath, 189, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("Y1-38209", FDateTime(2013, 11, 5), KeypadFilePath, 233, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("P1-43825", FDateTime(2016, 1, 18), KeypadFilePath, 276, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("A0-58397", FDateTime(2018, 6, 9), KeypadFilePath, 318, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("D9-65567", FDateTime(2022, 4, 27), KeypadFilePath, 365, "-r--r--r--", FOperatingSystemFileType::File, false);

	FileSystemAddFile("L7-48291", FDateTime(2003, 1, 12), KeypadFilePath, 1650, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("O2-77144", FDateTime(2004, 3, 8), KeypadFilePath, 1890, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("C0-33918", FDateTime(2005, 5, 21), KeypadFilePath, 2140, "-r--r--r--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("K1-90822", FDateTime(2006, 5, 30), KeypadFilePath, 2380, "-r--r--r--", FOperatingSystemFileType::File, false);

	FileSystemAddFile("Q4-91827", FDateTime(2010, 2, 11), KeypadFilePath, 512, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("M8-66219", FDateTime(2014, 8, 3), KeypadFilePath, 721, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("T2-19482", FDateTime(2019, 5, 19), KeypadFilePath, 1489, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("B6-77120", FDateTime(2008, 9, 7), KeypadFilePath, 2331, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("Z1-55092", FDateTime(2015, 12, 25), KeypadFilePath, 1987, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("L3-88421", FDateTime(2017, 3, 30), KeypadFilePath, 1675, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("R9-10293", FDateTime(2020, 10, 14), KeypadFilePath, 2591, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("H5-66734", FDateTime(2012, 6, 6), KeypadFilePath, 903, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("C8-33912", FDateTime(2016, 11, 11), KeypadFilePath, 1450, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("W2-77419", FDateTime(2023, 2, 8), KeypadFilePath, 2122, "-rwxr-xr--", FOperatingSystemFileType::File, false);

	FileSystemAddFile("F7-44102", FDateTime(2011, 4, 1), KeypadFilePath, 1803, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("N3-55287", FDateTime(2007, 7, 19), KeypadFilePath, 1345, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("U6-99123", FDateTime(2024, 1, 13), KeypadFilePath, 2750, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("J2-11098", FDateTime(2018, 9, 2), KeypadFilePath, 1662, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("X5-33771", FDateTime(2013, 5, 27), KeypadFilePath, 2015, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("D4-77331", FDateTime(2021, 6, 6), KeypadFilePath, 2488, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("S1-99872", FDateTime(2009, 11, 9), KeypadFilePath, 1177, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("G8-66220", FDateTime(2010, 1, 21), KeypadFilePath, 2233, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("P6-44188", FDateTime(2012, 12, 12), KeypadFilePath, 1544, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("K3-77110", FDateTime(2025, 3, 3), KeypadFilePath, 2644, "-rwxr-xr--", FOperatingSystemFileType::File, false);

	FileSystemAddFile("A7-90211", FDateTime(2017, 7, 7), KeypadFilePath, 1788, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("E4-33109", FDateTime(2019, 9, 9), KeypadFilePath, 1933, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("I2-88444", FDateTime(2014, 4, 4), KeypadFilePath, 1202, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("O9-77231", FDateTime(2022, 8, 8), KeypadFilePath, 2311, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("V1-66543", FDateTime(2008, 3, 12), KeypadFilePath, 2103, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("Y6-44892", FDateTime(2015, 5, 5), KeypadFilePath, 1377, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("T8-11992", FDateTime(2016, 6, 16), KeypadFilePath, 1899, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("R5-66789", FDateTime(2020, 7, 20), KeypadFilePath, 2455, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("L1-22298", FDateTime(2013, 2, 2), KeypadFilePath, 1750, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("C7-91919", FDateTime(2024, 4, 4), KeypadFilePath, 2875, "-rwxr-xr--", FOperatingSystemFileType::File, false);

	FileSystemAddFile("B2-55119", FDateTime(2011, 10, 10), KeypadFilePath, 1600, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("H9-77222", FDateTime(2018, 8, 18), KeypadFilePath, 1991, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("N4-44123", FDateTime(2021, 1, 1), KeypadFilePath, 2144, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("Z7-88001", FDateTime(2012, 2, 14), KeypadFilePath, 1711, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("U3-11882", FDateTime(2007, 6, 30), KeypadFilePath, 1400, "-rwxr-xr--", FOperatingSystemFileType::File, false);
	FileSystemAddFile("J8-77312", FDateTime(2025, 5, 5), KeypadFilePath, 2960, "-rwxr-xr--", FOperatingSystemFileType::File, false);
}

FString AOperatingSystem::GenerateLotterySequence()
{
	TSet<int32> Numbers;

	while (Numbers.Num() < 5)
	{
		Numbers.Add(FMath::RandRange(1, 60));
	}

	TArray<int32> SortedNumbers = Numbers.Array();
	SortedNumbers.Sort();

	FString Result = "";

	for (int32 i = 0; i < SortedNumbers.Num(); i++)
	{
		Result += FString::Printf(TEXT("%02d"), SortedNumbers[i]);
		Result += "-";
	}

	int32 Powerball = FMath::RandRange(1, 26);
	Result += FString::Printf(TEXT("%02d"), Powerball);

	return Result;
}

FDateTime AOperatingSystem::GenerateRandomDate()
{
	const FDateTime StartDate(2001, 1, 1);
	const FDateTime EndDate(2026, 1, 1);

	const int32 TotalDays = (EndDate - StartDate).GetDays();
	const int32 RandomDays = FMath::RandRange(0, TotalDays - 1);

	return StartDate + FTimespan(RandomDays, 0, 0, 0);
}

void AOperatingSystem::GenerateLotteryFiles()
{
	const int32 FileCount = 200;

	for (int32 i = 0; i < FileCount; i++)
	{
		FString Sequence = GenerateLotterySequence();
		FDateTime RandomDate = GenerateRandomDate();

		FileSystemAddFile(
			Sequence,
			RandomDate,
			"/winningsequence",
			0,
			"-r--r--r--",
			FOperatingSystemFileType::File,
			false
		);
	}
}

ATerminalSystem* AOperatingSystem::GetTerminalSystem()
{
	return TerminalSystem;
}