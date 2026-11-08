// Copyright Modfest Studios
#include "Online/ServerBrowserSearchResult.h"
#include "Subsystems/ServerSubsystem.h"

//========================================================FUNCTIONS========================================================

//=====================SERVER INFO=====================
FString UServerBrowserSearchResult::GetServerName() const
{
	return ServerName;
}

int32 UServerBrowserSearchResult::GetPing() const
{
	return Ping;
}

int32 UServerBrowserSearchResult::GetCurrentPlayers() const
{
	return CurrentPlayers;
}

int32 UServerBrowserSearchResult::GetMaxPlayers() const
{
	return MaxPlayers;
}

FString UServerBrowserSearchResult::GetServerId() const
{
	return ServerId;
}

bool UServerBrowserSearchResult::IsStale() const
{
	return bIsStale;
}

float UServerBrowserSearchResult::GetSecondsSinceLastSeen() const
{
	return SecondsSinceLastSeen;
}

FName UServerBrowserSearchResult::GetProviderName() const
{
	return ProviderName;
}

EServerConnectionType UServerBrowserSearchResult::GetConnectionType() const
{
	return ConnectionType;
}

//=====================CACHE UPDATES=====================
void UServerBrowserSearchResult::InitOnlineResult(const FOnlineSessionSearchResult& Result)
{
	SearchResult = Result;
	ServerId = Result.GetSessionIdStr();
	Ping = Result.PingInMs;
	MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
	CurrentPlayers = FMath::Max(0, MaxPlayers - Result.Session.NumOpenPublicConnections);
	ServerName.Empty();
	Result.Session.SessionSettings.Get(UServerSubsystem::SERVER_NAME, ServerName);
	bIsStale = false;
	SecondsSinceLastSeen = 0.0f;
	LastSeenAt = FPlatformTime::Seconds();
}
