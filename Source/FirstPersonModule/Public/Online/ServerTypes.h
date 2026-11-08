// Copyright Modfest Studios
#pragma once

#include "CoreMinimal.h"
#include "ServerTypes.generated.h"

class UServerBrowserSearchResult;

UENUM(BlueprintType)
enum class EServerConnectionType : uint8
{
	Direct,
	LAN,
	Online
};

UENUM(BlueprintType)
enum class EServerState : uint8
{
	Idle,
	Creating,
	Joining,
	TravelPending,
	Hosting,
	Connected,
	Leaving,
	Recovering,
	RecoveryFailed
};

UENUM(BlueprintType)
enum class EServerSearchState : uint8
{
	Idle,
	Searching,
	Cancelling
};

UENUM(BlueprintType)
enum class EServerOperation : uint8
{
	Search,
	Host,
	Join,
	Leave,
	Stop
};

UENUM(BlueprintType)
enum class EServerError : uint8
{
	None,
	InvalidRequest,
	Unsupported,
	ProviderUnavailable,
	OperationInProgress,
	SearchFailed,
	TimedOut,
	Cancelled,
	ConnectionFailed,
	NotLoggedIn,
	ServerFull,
	SessionNotFound,
	CleanupFailed
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerOperationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Server")
		FGuid RequestId;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		EServerOperation Operation = EServerOperation::Search;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		EServerError Error = EServerError::None;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		FText Message;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		bool bSucceeded = false;
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerHostSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		EServerConnectionType ConnectionType = EServerConnectionType::LAN;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		FString ServerName = TEXT("A Really Cool Server");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server", meta = (ClampMin = "1"))
		int32 MaxPlayers = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		bool bPubliclyListed = true;
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerSearchSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		EServerConnectionType ConnectionType = EServerConnectionType::LAN;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server", meta = (ClampMin = "1"))
		int32 MaxResults = 100;
	// Watchdog for an entire provider operation, not the LAN beacon response window.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server", meta = (ClampMin = "1.0"))
		float TimeoutSeconds = 15.0f;
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerJoinRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		EServerConnectionType ConnectionType = EServerConnectionType::LAN;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		TObjectPtr<UServerBrowserSearchResult> Server = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
		FString Address;
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Server")
		FString ServerName;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		EServerConnectionType ConnectionType = EServerConnectionType::LAN;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		int32 CurrentPlayers = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		int32 MaxPlayers = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		bool bPubliclyListed = false;
};

USTRUCT(BlueprintType)
struct FIRSTPERSONMODULE_API FServerCapabilities
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Server")
		bool bLANDiscoveryAvailable = false;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		bool bOnlineSessionsAvailable = false;
	// Discovery availability does not imply the project's net driver supports IP travel.
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		FName LANProvider = NAME_None;
	UPROPERTY(BlueprintReadOnly, Category = "Server")
		FName OnlineProvider = NAME_None;
};
