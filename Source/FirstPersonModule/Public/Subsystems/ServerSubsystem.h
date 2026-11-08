// Copyright Modfest Studios
#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/ServerTypes.h"
#include "ServerSubsystem.generated.h"

class UServerBrowserSearchResult;
struct FWorldContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerBrowserUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerOperationCompleted, const FServerOperationResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerStateChanged, EServerState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerSearchStateChanged, EServerSearchState, State);

// Kept for existing Blueprint pins. New callers can use GetServerSessionInfo().
USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString ServerName;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString SessionType;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		int32 OpenConnections = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		int32 MaxConnections = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		bool bIsAdvertising = false;
	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
		FString ServerStatus;
};

UCLASS()
class FIRSTPERSONMODULE_API UServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class FServerDiscoveryAutomationTest;
	friend class FServerLifecycleAutomationTest;

	//========================================================PROPERTIES=======================================================
private:
	//=====================LEGACY INPUTS=====================
	/** Default name used only by the deprecated HostLANSession wrapper. Prefer FServerHostSettings. */
	UPROPERTY(BlueprintGetter = GetServerName, BlueprintSetter = SetServerName, Category = "Server Subsystem|Legacy Settings", meta = (AllowPrivateAccess = "true"))
		FString ServerName = TEXT("A Really Cool Server");
	/** Reserved legacy password input. A nonempty value rejects hosting until authentication is implemented. */
	UPROPERTY(BlueprintGetter = GetPassword, BlueprintSetter = SetPassword, Category = "Server Subsystem|Legacy Settings", meta = (AllowPrivateAccess = "true"))
		FString Password;
	/** Endpoint used by the deprecated join wrapper when no browser result is selected. */
	UPROPERTY(BlueprintGetter = GetDirectConnectURL, BlueprintSetter = SetDirectConnectURL, Category = "Server Subsystem|Legacy Settings", meta = (AllowPrivateAccess = "true"))
		FString DirectConnectURL;

	//=====================BROWSER STATE=====================
	/** True while the provider still owns a search, including cancellation in progress. */
	UPROPERTY(BlueprintGetter = IsSearchingForServers, Category = "Server Subsystem|Browsing", meta = (AllowPrivateAccess = "true"))
		bool bSearchingForServers = false;
	/** Whether the automatic LAN discovery loop is enabled, even between searches. */
	UPROPERTY(BlueprintGetter = IsLANDiscoveryActive, Category = "Server Subsystem|Browsing", meta = (AllowPrivateAccess = "true"))
		bool bLANDiscoveryActive = false;
	/** Current search lifecycle; independent of the hosting/connection lifecycle. */
	UPROPERTY(BlueprintGetter = GetSearchState, Category = "Server Subsystem|Browsing", meta = (AllowPrivateAccess = "true"))
		EServerSearchState SearchState = EServerSearchState::Idle;
	/** Outcome of the most recently completed search, including cancellation and timeout. */
	UPROPERTY(BlueprintGetter = GetLastSearchResult, Category = "Server Subsystem|Browsing", meta = (AllowPrivateAccess = "true"))
		FServerOperationResult LastSearchResult;
	/** Private cache storage. Blueprints must call GetSearchResults rather than read this property. */
	UPROPERTY()
		TArray<UServerBrowserSearchResult*> SearchResults;
	/** Selected cache entry; cleared when that entry expires or browser modes change. */
	UPROPERTY()
		TObjectPtr<UServerBrowserSearchResult> ServerToJoin;

	//=====================SEARCH OWNERSHIP=====================
	IOnlineSessionPtr SearchInterface; // Provider that owns the current search/cancellation callbacks.
	TSharedPtr<FOnlineSessionSearch> SessionSearch; // Kept alive until the provider releases the search.
	FServerSearchSettings ActiveSearchSettings; // Immutable settings for the in-flight request.
	FServerSearchSettings RequestedSearchSettings; // Settings to use for the next manual/coalesced refresh.
	FGuid ActiveSearchId; // Rejects callbacks belonging to an older search.
	FName SearchProvider = NAME_None; // Part of each cached result's identity.
	EServerError PendingSearchError = EServerError::None; // Final reason to report after cancellation completes.
	double SearchStartedAt = 0.0; // Monotonic watchdog start time.
	double NextSearchAt = 0.0; // Monotonic time when the automatic loop may search again.
	int32 ConsecutiveSearchFailures = 0; // Controls automatic refresh backoff.
	bool bRefreshPending = false; // Coalesces multiple refresh calls into one follow-up search.
	bool bPublishingSearchResult = false; // Prevents recursive provider searches inside UI callbacks.

	//=====================SESSION OWNERSHIP=====================
	/** Current hosting, connection, leaving or recovery state. TravelPending is not success. */
	UPROPERTY(BlueprintGetter = GetServerState, Category = "Server Subsystem|Sessions", meta = (AllowPrivateAccess = "true"))
		EServerState ServerState = EServerState::Idle;
	IOnlineSessionPtr SessionInterface; // Provider selected for the active host/join lifecycle.
	FServerHostSettings ActiveHostSettings; // Hosting settings retained across map travel.
	FGuid SessionRequestId; // Identity of the operation that owns session callbacks.
	FGuid PendingLeaveRequestId; // Separate leave identity while an uncancellable create/join finishes.
	FString PendingMapURL; // Sanitized host map to load after successful session creation.
	EServerConnectionType ActiveConnectionType = EServerConnectionType::Direct;
	EServerOperation ActiveOperation = EServerOperation::Host;
	FName ActiveProvider = NAME_None; // None for direct connections.
	double OperationStartedAt = 0.0; // Monotonic session-operation watchdog start time.
	bool bSessionAbandoned = false; // Late create/join completion must clean up instead of travel.
	bool bOwnsSession = false; // Resolving a shared interface alone never grants cleanup ownership.
	bool bReturningToMenu = false; // Waiting for a fresh standalone world after cleanup.
	bool bOperationReported = false; // Ensures one completion notification per accepted operation.
	bool bServerStateDirty = false; // Defers state broadcasts until the ticker finishes its mutations.
	TWeakObjectPtr<UWorld> TravelSourceWorld; // Distinguishes completed map travel from the source world.

	//=====================QUEUED REQUESTS=====================
	/** Keeps the selected result alive while leaving the previous connection. */
	UPROPERTY()
		FServerJoinRequest QueuedJoinRequest;
	FServerHostSettings QueuedHostSettings; // Host request to execute after leaving completes.
	bool bHostQueued = false;
	bool bJoinQueued = false;

	//=====================CALLBACK REGISTRATION=====================
	FDelegateHandle FindDelegateHandle;
	FDelegateHandle CancelFindDelegateHandle;
	FDelegateHandle CreateDelegateHandle;
	FDelegateHandle JoinDelegateHandle;
	FDelegateHandle DestroyDelegateHandle;
	FDelegateHandle WorldTearDownHandle;
	FDelegateHandle NetworkFailureHandle;
	FDelegateHandle TravelFailureHandle;
	FTSTicker::FDelegateHandle DiscoveryTickerHandle; // Survives map travel; drives discovery and session recovery.
	bool bDeinitializing = false; // Rejects requests and late callbacks during subsystem shutdown.

public:
	//=====================ADVERTISEMENT KEYS=====================
	// Native protocol constants, not mutable gameplay settings or Blueprint state.
	static const FName SERVER_NAME;
	static const FName GAME_VERSION;
	static const FName GAME_MODE;

	//=====================EVENTS=====================
	/** Re-read GetSearchResults when entries are added, updated, expired, or cleared. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerBrowserUpdate OnSearchResultsUpdated;
	/** Search entered Searching, Cancelling, or Idle. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerSearchStateChanged OnSearchStateChanged;
	/** A provider search completed, including successful empty results, cancellation, or failure. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerOperationCompleted OnSessionSearchCompleted;
	/** Validation, connection, search, or cleanup failure suitable for a UI error message. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerOperationCompleted OnServerOperationFailed;
	/** Hosting/connection state changed; broadcast at the end of the subsystem tick. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerStateChanged OnServerStateChanged;
	/** An accepted host/join/leave/stop operation finished, failed, or was cancelled. */
	UPROPERTY(BlueprintAssignable, Category = "Server Subsystem|Events")
		FOnServerOperationCompleted OnServerOperationCompleted;

	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================



	//=====================INITIALIZATION=====================
	public:
	/** Engine lifecycle override: registers the discovery ticker and world/network failure callbacks. Not a gameplay Blueprint action. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	/** Engine lifecycle override: stops discovery, unbinds callbacks, releases transport ownership, and cleans up owned sessions. */
	virtual void Deinitialize() override;

	//=====================LEGACY SETTINGS=====================
	/** Read the legacy ServerName input; this does not describe an active session. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Legacy Settings")
		FString GetServerName() const;
	/** Set the legacy ServerName input. Does not update an active session. */
	UFUNCTION(BlueprintSetter, Category = "Server Subsystem|Legacy Settings")
		void SetServerName(const FString& Value);
	/** Read the legacy Password input; this does not describe an active session. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Legacy Settings")
		FString GetPassword() const;
	/** Set the legacy Password input. Does not update an active session. */
	UFUNCTION(BlueprintSetter, Category = "Server Subsystem|Legacy Settings")
		void SetPassword(const FString& Value);
	/** Read the legacy DirectConnectURL input; this does not describe an active session. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Legacy Settings")
		FString GetDirectConnectURL() const;
	/** Set the legacy DirectConnectURL input. Does not update an active session. */
	UFUNCTION(BlueprintSetter, Category = "Server Subsystem|Legacy Settings")
		void SetDirectConnectURL(const FString& Value);


	//======================
	//=======SESSIONS=======
	//======================

	/** Legacy snapshot for existing Blueprint pins. Prefer GetServerSessionInfo for the typed connection mode. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Sessions")
		FSessionInfo GetCurrentSessionInfo();
	/** Read advertised session details and the current connection mode. Use GetServerState to determine connection status. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Sessions")
		FServerSessionInfo GetServerSessionInfo() const;
	/** Read available discovery providers and online login readiness. Availability does not guarantee a successful connection. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Sessions")
		FServerCapabilities GetServerCapabilities() const;
	/** Current session lifecycle state. Hosting/Connected indicate established connections. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Sessions")
		EServerState GetServerState() const;
	// Legacy interface-availability query; not an active-session test.
	/** Legacy provider-interface check only. This does not mean the player is connected; use GetServerState. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Sessions")
		bool IsSessionValid();

	//=====================
	//=======HOSTING=======
	//=====================

	/** Host the next map selected in MapSubsystem using LAN, Direct, or Online. If connected, leave first and queue the new host request. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void StartServer(const FServerHostSettings& Settings);
	/** Legacy LAN host wrapper using the stored ServerName. Prefer StartServer with explicit host settings. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN", meta = (DeprecatedFunction, DeprecationMessage = "Use StartServer with LAN connection type."))
		void HostLANSession();
private:
	/** Provider callback: reject stale requests, clean up abandoned creates, or begin loading the listen map. */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful, FGuid RequestId);

	//============================
	//=======LEAVING & RECOVERY===
	//============================
public:
	/** Leave a client or hosted session and return to the configured standalone menu. Cancels any queued host/join request. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void LeaveSession();
	/** Stop a hosted server and return to the standalone menu. Clients should use LeaveSession. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void StopServer();
private:
	/** Start leave/stop cleanup, or wait for an uncancellable create/join callback before destroying its result. */
	void BeginLeave(EServerOperation Operation);
	/** Destroy only the session this subsystem owns. Retain ownership until the provider confirms completion. */
	void DestroyActiveSession();
	/** Provider callback: release a successfully destroyed session or expose RecoveryFailed so cleanup can be retried. */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful, FGuid RequestId);
	/** Release session/transport references and request absolute standalone travel. Idle waits for the new menu world. */
	void ReturnToMenu();
	/** Publish one completion outcome for the active operation; failures also notify OnServerOperationFailed. */
	void FinishSessionOperation(EServerError Error);
	/** Record a connection/travel failure and begin cleanup. An outstanding provider create/join must finish before destruction. */
	void FailSessionOperation(EServerError Error);
	/** Handle game/pending connection failures belonging to this Game Instance; ignore other worlds and beacon drivers. */
	void OnNetworkFailure(UWorld* World, UNetDriver* Driver, ENetworkFailure::Type Failure, const FString& Message);
	/** Handle this Game Instance's map-travel failure, including a failed return to the menu. */
	void OnTravelFailure(UWorld* World, ETravelFailure::Type Failure, const FString& Message);

	//===================================
	//=============SEARCHING=============
	//===================================

public:
	/** True while a provider owns a search or cancellation request. This is separate from automatic LAN discovery being enabled. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Browsing")
		bool IsSearchingForServers();
	/** True when the automatic LAN loop is enabled, including time between searches. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Browsing")
		bool IsLANDiscoveryActive() const;
	/** Current search state, including cancellation still owned by the provider. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Browsing")
		EServerSearchState GetSearchState() const;
	/** Copy of the most recently completed search outcome. */
	UFUNCTION(BlueprintGetter, Category = "Server Subsystem|Browsing")
		FServerOperationResult GetLastSearchResult() const;

	/** Count cached browser entries, including stale entries that have not expired. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Browsing")
		int32 GetNumOfServersFound();

	// C++ receives a read-only array reference; Blueprints receive an array copy.
	/** Read cached browser entries. C++ gets a const array reference; Blueprints receive an array copy. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Browsing")
		const TArray<UServerBrowserSearchResult*>& GetSearchResults() const;

	/** Search LAN or Online with explicit settings. Calls during an existing search coalesce into one follow-up refresh; Direct has no browser. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void FindSessions(const FServerSearchSettings& Settings);
	/** Repeat the last requested search settings. Multiple calls during a search coalesce into one follow-up request. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void RefreshSessions();
	/** Stop automatic discovery and request cancellation of the current LAN or Online search. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void CancelSessionSearch();
	/** Start periodic LAN searches using Server Settings, retaining cached results between refreshes. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void StartLANDiscovery();
	/** Disable the automatic LAN loop, clear queued refreshes, and cancel any current search. Cached results remain until expiry. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void StopLANDiscovery();
	/** Legacy one-shot LAN search using Server Settings. Prefer FindSessions or StartLANDiscovery. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN", meta = (DeprecatedFunction, DeprecationMessage = "Use FindSessions or StartLANDiscovery."))
		void FindLANSessions();

	//=====================
	//=======JOINING=======
	//=====================
public:
	/** Join a cached LAN/Online result or a Direct address. The result's provider must match its requested mode. Travel success is asynchronous. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions")
		void JoinSession(const FServerJoinRequest& Request);
	/** Legacy wrapper: join the selected browser entry, or use DirectConnectURL when no entry is selected. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions|LAN", meta = (DeprecatedFunction, DeprecationMessage = "Use JoinSession."))
		void JoinLANSession();
	/** Legacy direct-IP convenience wrapper. Prefer JoinSession with Direct connection type and an Address. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Sessions", meta = (DeprecatedFunction, DeprecationMessage = "Use JoinSession with Direct connection type and an Address."))
		void JoinDirectConnect(FString ServerURL);
	/** True when the selected entry still belongs to this browser cache and contains valid provider session data. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Browsing")
		bool HasSelectedServerToJoin();
	/** Read the selected browser entry, or null when nothing is selected. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Browsing")
		UServerBrowserSearchResult* GetSelectedServer();
	/** Select an entry from this subsystem's current browser cache. Foreign or invalid entries are ignored. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void SetServerSelection(UServerBrowserSearchResult* SearchResult);
	/** Clear the selected entry without changing search results or cancelling discovery. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void ClearServerSelection();
	/** Clear cached entries and selection, then notify the UI. Does not cancel a provider search already in progress. */
	UFUNCTION(BlueprintCallable, Category = "Server Subsystem|Browsing")
		void ClearServerBrowserResults();

private:
	/** Provider callback: translate join failures or resolve the connection address and begin client travel. */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, FGuid RequestId);


	//============================
	//=======DISCOVERY CALLBACKS===
	//============================


	/** Provider callback: ignore stale search IDs and finish the search using its success or pending cancellation reason. */
	void OnFindSessionsComplete(bool bWasSuccessful, FGuid RequestId);
	/** Provider callback: release search ownership only when cancellation succeeds; otherwise await the original search callback. */
	void OnCancelFindSessionsComplete(bool bWasSuccessful, FGuid RequestId);
	/** Request provider cancellation and retain ownership until completion, preserving the requested cancellation/timeout reason. */
	void CancelActiveSearch(EServerError Reason);
	/** Merge successful provider results, schedule retry timing, release search ownership, and publish browser/search events. */
	void CompleteSearch(FGuid RequestId, EServerError Error);
	/** Unbind search delegates and discard the active provider request. Does not clear cached browser entries. */
	void ReleaseSearch();
	/** Merge returned sessions by provider, connection mode, and session ID, preserving existing entry objects and selection. */
	void MergeSearchResults(double Now);
	/** Mark unseen entries stale and expire old entries using monotonic time. Returns whether the visible cache changed. */
	bool AgeSearchResults(double Now);
	/** Core ticker callback: advance travel/recovery, enforce timeouts, age results, run queued searches, and flush state events. */
	bool TickDiscovery(float DeltaSeconds);
	/** Stop discovery when this Game Instance's world begins teardown. Session state survives intentional map travel. */
	void OnWorldTearDown(UWorld* World);

	//===========================
	//=========UTILITIES=========
	//===========================
public:
	/** True for an authority world, including standalone play. Use GetServerState to check whether an online/LAN server is actually hosted. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Utilities")
		bool IsServer();
	/** Get this world's FirstPersonGame authority GameMode, or null on a client or when another GameMode type is used. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Utilities")
		class AFirstPersonGame* GetGameMode();
	/** Get this world's FirstPersonGameState, or null if unavailable or a different type is used. */
	UFUNCTION(BlueprintPure, Category = "Server Subsystem|Utilities")
		class AFirstPersonGameState* GetGameState();
private:
	/** Resolve the world-aware Null session provider explicitly, regardless of the game's default online service. */
	IOnlineSessionPtr ResolveLANInterface() const;
	/** Check that this subsystem is idle in a valid standalone world where discovery may run. */
	bool CanSearchSessions() const;
	/** Resolve the requested session provider. Direct uses no provider; unsupported Online providers return null. */
	IOnlineSessionPtr ResolveSessionInterface(EServerConnectionType Type, FName Provider = NAME_None) const;
	/** Check that local user 0 is signed in to the supported Online provider and has a platform identity. */
	bool IsOnlineUserReady(FName Provider) const;
	/** Temporarily select IP or SteamSockets for upcoming travel. Refuse competing ownership and incompatible socket/driver overrides. */
	bool AcquireTransport(EServerConnectionType Type);
	/** Restore the previous GameNetDriver definition only if this subsystem owns the temporary selection. */
	void ReleaseTransport();
	/** Install the new request identity, mode, and watchdog, and reset completion/recovery flags before starting provider work. */
	void BeginSessionOperation(FGuid RequestId, EServerOperation Operation, EServerConnectionType Type);
	/** Load a fresh listen map for hosting, or request client travel to the resolved server endpoint. */
	void BeginSessionTravel(const FString& URL);
	/** Unbind create/join/destroy callbacks from the active provider without altering the provider session itself. */
	void ClearSessionDelegates();
	/** Update internal session state and defer its Blueprint event until the ticker finishes mutations. */
	void SetServerState(EServerState State);
	/** Update internal search flags and broadcast the new search state. */
	void SetSearchState(EServerSearchState State);
	/** Build the Blueprint-facing request outcome and localized message from an operation/error pair. */
	FServerOperationResult MakeOperationResult(FGuid RequestId, EServerOperation Operation, EServerError Error) const;
	/** Log and broadcast a rejected request or cleanup error without starting a new operation. */
	void ReportFailure(FGuid RequestId, EServerOperation Operation, EServerError Error);
};
