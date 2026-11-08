// Copyright Modfest Studios
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Online/ServerTypes.h"
#include "ServerSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Server Settings"))
class FIRSTPERSONMODULE_API UServerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	//========================================================PROPERTIES=======================================================
	// Editable project configuration, unlike the subsystem's private runtime state.
	// LAN always uses NULL; Phase 4 implements Steam for Online sessions.
	/** Online provider configured for session discovery and joining. Phase 4 supports STEAM. */
	UPROPERTY(Config, EditAnywhere, Category = "Online Sessions")
		FName OnlineProvider = TEXT("STEAM");
	/** Watchdog duration for hosting, joining, leaving, and map travel. */
	UPROPERTY(Config, EditAnywhere, Category = "Sessions", meta = (ClampMin = "5.0"))
		float OperationTimeoutSeconds = 60.0f;
	// Empty uses the project game default map. Choose a standalone menu map.
	/** Configured standalone return map. An empty path uses the project game default map. */
	UPROPERTY(Config, EditAnywhere, Category = "Sessions", meta = (AllowedClasses = "/Script/Engine.World"))
		FSoftObjectPath ReturnMap;
	/** Default search parameters used by automatic LAN discovery and legacy LAN search. */
	UPROPERTY(Config, EditAnywhere, Category = "LAN Discovery")
		FServerSearchSettings LANSearch;
	/** Delay after a completed LAN search before automatic discovery searches again. */
	UPROPERTY(Config, EditAnywhere, Category = "LAN Discovery", meta = (ClampMin = "0.1"))
		float RefreshInterval = 3.0f;
	/** Maximum delay between automatic LAN retries after repeated failures. */
	UPROPERTY(Config, EditAnywhere, Category = "LAN Discovery", meta = (ClampMin = "1.0"))
		float MaximumRetryInterval = 30.0f;
	/** Age at which an unseen browser result is marked stale. */
	UPROPERTY(Config, EditAnywhere, Category = "LAN Discovery", meta = (ClampMin = "0.1"))
		float StaleAfterSeconds = 12.0f;
	/** Age at which an unseen result is removed; runtime uses at least StaleAfterSeconds. */
	UPROPERTY(Config, EditAnywhere, Category = "LAN Discovery", meta = (ClampMin = "1.0"))
		float ExpireAfterSeconds = 30.0f;

	//========================================================FUNCTIONS========================================================
	//=====================CONFIGURATION=====================
	/** Online provider configured for session discovery and joining. Phase 4 supports STEAM. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		FName GetOnlineProvider() const;
	/** Watchdog duration for hosting, joining, leaving, and map travel. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		float GetOperationTimeoutSeconds() const;
	/** Configured standalone return map. An empty path uses the project game default map. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		FSoftObjectPath GetReturnMap() const;
	/** Default search parameters used by automatic LAN discovery and legacy LAN search. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		FServerSearchSettings GetLANSearch() const;
	/** Delay after a completed LAN search before automatic discovery searches again. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		float GetRefreshInterval() const;
	/** Maximum delay between automatic LAN retries after repeated failures. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		float GetMaximumRetryInterval() const;
	/** Age at which an unseen browser result is marked stale. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		float GetStaleAfterSeconds() const;
	/** Age at which an unseen result is removed; runtime uses at least StaleAfterSeconds. */
	UFUNCTION(BlueprintPure, Category = "Server Settings")
		float GetExpireAfterSeconds() const;
};
