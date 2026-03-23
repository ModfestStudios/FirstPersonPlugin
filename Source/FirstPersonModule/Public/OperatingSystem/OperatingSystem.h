// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
};


/**
 * 
 */
UCLASS(Blueprintable)
class FIRSTPERSONMODULE_API AOperatingSystem : public AInfo
{
	GENERATED_BODY()
public:

	/*core*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Operating System|Core")
		EOperatingSystemMainState SystemState;

	/*post*/
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test")
		bool bEnablePOST;
	UPROPERTY(EditAnywhere, Category = "Boot|Power-On Self Test", meta = (DisplayName = "Power-On Self Test Messages"))
		TArray<FOperatingSystemPOST> PoweronSelfTestMessages;

	/*system variables*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Variables")
		TArray<FOperatingSystemVariable> SystemVariables;


	/*operating system*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Core")
		FText OperatingSystemName = NSLOCTEXT("OperatingSystem", "OSName", "Rabbit Hole");

	/*directories*/
protected:
	UPROPERTY(EditAnywhere, Category = "Operating System|Directories")
		TArray<FOperatingSystemDirectory> Directories;


protected:
	/*list of users on this machine*/
	UPROPERTY(EditAnywhere, Category = "Operating System|Users")
		TArray<FOperatingSystemUser> Users;
	UPROPERTY(EditAnywhere, Category = "Operating System|Users")
		FString CurrentUser;

	//=============================================================================================================================================================
	//==========================================================================FUNCTIONS==========================================================================
	//=============================================================================================================================================================

	AOperatingSystem();



	UFUNCTION(BlueprintCallable, Category = "Operating System|Boot Loader")
		virtual void Boot();
	
	UFUNCTION()
		virtual void BeginBootSequence();

	UFUNCTION()
		virtual void InitiateBIOS();


	UFUNCTION()
		virtual void EndBootSequence();

	//=========================
	//=======directories=======
	//=========================
public:
	UFUNCTION(BlueprintCallable, Category = "Operating System|Directories")
		TArray<FOperatingSystemDirectory> GetAllRootDirectories();


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

};
