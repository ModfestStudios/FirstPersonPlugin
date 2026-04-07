// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/FileHelper.h" //For ARG file export
#include "Misc/Paths.h" //For ARG file export
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "OperatingSystem.generated.h"

USTRUCT(BlueprintType)
struct FOperatingSystemUser
{
	GENERATED_BODY()
public:


	UPROPERTY(EditAnywhere, Category = "User")
		FString Username;
	UPROPERTY(EditAnywhere, Category = "User")
		FString Password;


	FOperatingSystemUser()
	{
	}
	FOperatingSystemUser(FString Username, FString Password)
	{
		this->Username = Username;
		this->Password = Password;
	}
};

UENUM(BlueprintType)
enum class EOperatingSystemMainState : uint8
{
	PoweredOff,
	POST,
	BootLoader,
	LockScreen,
	Desktop
};

USTRUCT(BlueprintType)
struct FOperatingSystemVariable
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category = "System Variables")
		FName Name;
	UPROPERTY(EditAnywhere, Category = "System Variables")
		FName Value;
};

USTRUCT(BlueprintType)
struct FOperatingSystemPOST
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Power-On Self Test")
		FText Message;
	UPROPERTY(EditAnywhere, Category = "Power-On Self Test")
		float ExecuteTime;
};

//New file system in place of old directory structure (maintains files, directories, content, and path)
UENUM(BlueprintType)
enum class FOperatingSystemFileSortBy : uint8
{
	Name,
	Size,
	Date
};

UENUM(BlueprintType)
enum class FOperatingSystemFileType : uint8
{
	Directory,
	File
};

USTRUCT(BlueprintType)
struct FOperatingSystemFiles
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File System")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System")
	FDateTime Date;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System", meta = (ToolTip = "If stored in root (home), leave blank; If within a directory it must contain prefix of '/' for example a subdirectory TestDirectory of root would be '/TestDirectory'"))
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System")
	int32 Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System")
	FString Permissions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System")
	FOperatingSystemFileType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System")
	bool bIsHidden;

	//UDF1 is used to store properties about the file that might be relevant for activities (e.g. tagging a file)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File System", meta = (ToolTip = "UDF1 is used for generic notes/details of a file (e.g. tagging it as a specific puzzle piece"))
	FString UDF1;

	FOperatingSystemFiles()
	{
		Name = "";
		Date = FDateTime::Now();
		Path = "";
		Size = 0;
		Permissions = "";
		Type = FOperatingSystemFileType::File;
		bIsHidden = false;
		UDF1 = "";
	}

	FOperatingSystemFiles(const FString& InName, const FDateTime& InDate, const FString& InPath, const int32& InSize, const FString& InPermissions, FOperatingSystemFileType InType = FOperatingSystemFileType::File, bool InIsHidden = false, FString InUDF1 = "")
	{
		Name = InName;
		Date = InDate;
		Path = InPath;
		Size = InSize;
		Permissions = InPermissions;
		Type = InType;
		bIsHidden = InIsHidden;
		UDF1 = InUDF1;
	}

};

/*TODO: This is the original directory structure; keeping for now but we may opt to remove / move it to the File System class
USTRUCT(BlueprintType)
struct FOperatingSystemDirectory
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, Category = "Directory")
	int32 DirectoryID = INDEX_NONE;
	UPROPERTY(EditAnywhere, Category = "Directory")
	int32 ParentID = INDEX_NONE;

	UPROPERTY(EditAnywhere, Category = "Directory")
	FString Label;
	UPROPERTY(VisibleAnywhere, Category = "Directory")
	bool bHidden = false;
	UPROPERTY(VisibleAnywhere, Category = "Directory")
	FName Path;

	FOperatingSystemDirectory() {}
	FOperatingSystemDirectory(FString Label, int32 DirectoryID, int32 ParentId)
	{
		this->Label = Label;
		this->DirectoryID = DirectoryID;
		this->ParentID = ParentId;
	}
	
};*/


/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API AOperatingSystem : public AInfo
{
	GENERATED_BODY()
public:

protected:
	/*core*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Operating System|Core")
		EOperatingSystemMainState SystemState;
public:
	/*post*/
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test")
		bool bEnablePOST;
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test", meta = (DisplayName = "Power-On Self Test Messages"))
		TArray<FOperatingSystemPOST> PoweronSelfTestMessages;
public:
	/*system variables*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Variables")
		TArray<FOperatingSystemVariable> SystemVariables;
	UPROPERTY(EditAnywhere, Category = "Operating System|Variables", meta = (ToolTip = "Defines the root directory (shows in terminal) - is cosmetic only"))
		FString HomeDirectory;
	
	UPROPERTY(EditAnywhere, BluePrintReadOnly)
		FString ActiveWorkingDirectory; //Should only be set by C++ logic due to navigating file structure dependency; default is blank

	/*operating system*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Core")
		FText OperatingSystemName = NSLOCTEXT("OperatingSystem", "OSName", "Rabbit Hole");

/* TODO: Remove or reuse: old directories - retired for now
protected:
	UPROPERTY(EditAnywhere, Category = "Operating System|Directories")
		TArray<FOperatingSystemDirectory> Directories;
*/

protected:
	/*list of users on this machine*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Users")
		TArray<FOperatingSystemUser> Users;
	UPROPERTY(EditAnywhere, Category = "Operating System|Users")
		FString CurrentUser;

	
protected:
	/*the in-world actor (terminal) that owns this operating system and provides access to it from the world*/
	UPROPERTY()
		class ATerminalSystem* TerminalSystem;


protected:
	UPROPERTY()
		class UOperatingSystemWidget* Widget;


	//=============================================================================================================================================================
	//==========================================================================FUNCTIONS==========================================================================
	//=============================================================================================================================================================

	AOperatingSystem();

public:
	UFUNCTION()
		virtual void InitializeFromTerminal(class ATerminalSystem* Terminal);

	//==============
	//=====BOOT=====
	//==============

	UFUNCTION(BlueprintCallable, Category = "Operating System|Boot Loader")
		virtual void Boot();
	
	UFUNCTION()
		virtual void BeginBootSequence();

	UFUNCTION()
		virtual void InitiateBIOS();
	UFUNCTION()
		virtual void BeginPOST();


	UFUNCTION()
		virtual void EndBootSequence();


	//=========================
	//======FileSystem=========
	//=========================
public:
	UFUNCTION()
	FString GetActiveWorkingDirectory() const;

	UFUNCTION()
	FString GetHomeDirectory() const;

	UFUNCTION()
	virtual void SetActiveWorkingDirectory(const FString& NewWorkingDirectory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File System")
	TArray<FOperatingSystemFiles> SystemFiles;

	UFUNCTION(BlueprintCallable)
	void FileSystemAddFile(const FString& Name, const FDateTime& Date, const FString& FilePath, const int32& Size, const FString& Permissions, FOperatingSystemFileType Type, bool bIsHidden, FString UDF1 = "");

	UFUNCTION(BlueprintCallable)
	void FileSystemDeleteFile(const FString& Name, const FString& FilePath, FOperatingSystemFileType Type);

	UFUNCTION(BlueprintCallable)
	bool FileSystemCheckIfFileExists(const FString& Name, const FString& Path, FOperatingSystemFileType Type);

	UFUNCTION(BlueprintCallable)
	TArray<FOperatingSystemFiles> FileSystemListFiles(FOperatingSystemFileSortBy SortBy = FOperatingSystemFileSortBy::Name) const;

	UFUNCTION(BlueprintCallable)
	FOperatingSystemFiles FileSystemGetFile(const FString& Name, FString Path, FString UDF1);

	virtual void BeginPlay() override;

	//Lottery Sequence Generator
	FString GenerateLotterySequence();
	FDateTime GenerateRandomDate();
	void GenerateLotteryFiles();

/*TODO: Original directories system; retired for now; bring back if needed
	//=========================
	//=======directories=======
	//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Operating System|Directories")
		TArray<FOperatingSystemDirectory> GetAllRootDirectories();
*/

	//=====================
	//========users========
	//=====================
public:
	UFUNCTION(BlueprintCallable, Category = "Operating System|Users")
		void SetCurrentUser(FOperatingSystemUser User);
	UFUNCTION(BlueprintCallable, Category = "Operating System|Users")
		TArray<FOperatingSystemUser> GetAllUsers();
	UFUNCTION(BlueprintCallable, Category = "Operating System|Users")
		FString GetCurrentUser();


	//=====================
	//====ARG Functions====
	//=====================
public:
	UFUNCTION()
	FString ExportARGFile(const  FString& Name, const FString& FileContent);

	UFUNCTION()
	FString GetRealWorldUsername();

	//====================
	//=======SYSTEM=======
	//====================
	public:
		UFUNCTION(BlueprintPure, Category = "Operating System|System")
			EOperatingSystemMainState GetSystemState();

	
	//======================
	//=======TERMINAL=======
	//======================
	public:
		UFUNCTION(BlueprintPure, Category = "Operating System|Terminal System")
			class ATerminalSystem* GetTerminalSystem();
};