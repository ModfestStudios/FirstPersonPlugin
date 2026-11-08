// Copyright Modfest Studios
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OnlineSessionSettings.h"
#include "Online/ServerTypes.h"
#include "ServerBrowserSearchResult.generated.h"

UCLASS(BlueprintType)
class FIRSTPERSONMODULE_API UServerBrowserSearchResult : public UObject
{
	GENERATED_BODY()
	friend class UServerSubsystem;
	friend class FServerDiscoveryAutomationTest;

	//========================================================PROPERTIES=======================================================
private:
	/** Advertised display name of this server. */
	UPROPERTY(BlueprintGetter = GetServerName, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		FString ServerName;
	/** Provider-reported ping in milliseconds; some online providers may return an unavailable estimate. */
	UPROPERTY(BlueprintGetter = GetPing, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		int32 Ping = 0;
	/** Advertised occupied public slots; this is not an authoritative live player count. */
	UPROPERTY(BlueprintGetter = GetCurrentPlayers, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		int32 CurrentPlayers = 0;
	/** Advertised maximum public player slots. */
	UPROPERTY(BlueprintGetter = GetMaxPlayers, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		int32 MaxPlayers = 0;
	/** Provider session ID. Combine with provider and connection type when comparing entries. */
	UPROPERTY(BlueprintGetter = GetServerId, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		FString ServerId;
	/** True when this entry has not been seen within the configured stale interval. */
	UPROPERTY(BlueprintGetter = IsStale, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		bool bIsStale = false;
	// Monotonic process time; use SecondsSinceLastSeen for display.
	double LastSeenAt = 0.0;
	/** Elapsed time since this session was last returned by discovery. */
	UPROPERTY(BlueprintGetter = GetSecondsSinceLastSeen, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		float SecondsSinceLastSeen = 0.0f;

	// Joining must use the provider that returned this result.
	/** Provider that discovered this session and must perform its join. */
	UPROPERTY(BlueprintGetter = GetProviderName, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		FName ProviderName = NAME_None;
	/** Connection mode to use when building a join request for this entry. */
	UPROPERTY(BlueprintGetter = GetConnectionType, Category = "Server Info", meta = (AllowPrivateAccess = "true"))
		EServerConnectionType ConnectionType = EServerConnectionType::LAN;

	// Native provider payload, intentionally unavailable for external mutation or Blueprint calls.
	FOnlineSessionSearchResult SearchResult;

	//========================================================FUNCTIONS========================================================

	//=====================SERVER INFO=====================
public:
	/** Advertised display name of this server. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		FString GetServerName() const;
	/** Provider-reported ping in milliseconds; some online providers may return an unavailable estimate. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		int32 GetPing() const;
	/** Advertised occupied public slots; this is not an authoritative live player count. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		int32 GetCurrentPlayers() const;
	/** Advertised maximum public player slots. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		int32 GetMaxPlayers() const;
	/** Provider session ID. Combine with provider and connection type when comparing entries. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		FString GetServerId() const;
	/** True when this entry has not been seen within the configured stale interval. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		bool IsStale() const;
	/** Elapsed time since this session was last returned by discovery. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		float GetSecondsSinceLastSeen() const;
	/** Provider that discovered this session and must perform its join. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		FName GetProviderName() const;
	/** Connection mode to use when building a join request for this entry. */
	UFUNCTION(BlueprintGetter, Category = "Server Info")
		EServerConnectionType GetConnectionType() const;

	//=====================CACHE UPDATES=====================
private:
	/** Copy provider data into this cache entry and reset its age. Only the owning subsystem may initialize results. */
	void InitOnlineResult(const FOnlineSessionSearchResult& Result);
};
