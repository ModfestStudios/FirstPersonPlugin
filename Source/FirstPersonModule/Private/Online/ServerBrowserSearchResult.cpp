// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/ServerBrowserSearchResult.h"
#include "Subsystems/ServerSubsystem.h"

void UServerBrowserSearchResult::InitOnlineResult(const FOnlineSessionSearchResult& Result)
{
	SearchResult = Result;	
	Ping = Result.PingInMs;
	MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
	CurrentPlayers = MaxPlayers - Result.Session.NumOpenPublicConnections;
	
	/*metadata*/
	Result.Session.SessionSettings.Get(UServerSubsystem::SERVER_NAME, ServerName);
}
