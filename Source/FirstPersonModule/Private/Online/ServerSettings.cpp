// Copyright Modfest Studios
#include "Online/ServerSettings.h"

//========================================================FUNCTIONS========================================================
//=====================CONFIGURATION=====================
FName UServerSettings::GetOnlineProvider() const
{
	return OnlineProvider;
}

float UServerSettings::GetOperationTimeoutSeconds() const
{
	return OperationTimeoutSeconds;
}

FSoftObjectPath UServerSettings::GetReturnMap() const
{
	return ReturnMap;
}

FServerSearchSettings UServerSettings::GetLANSearch() const
{
	return LANSearch;
}

float UServerSettings::GetRefreshInterval() const
{
	return RefreshInterval;
}

float UServerSettings::GetMaximumRetryInterval() const
{
	return MaximumRetryInterval;
}

float UServerSettings::GetStaleAfterSeconds() const
{
	return StaleAfterSeconds;
}

float UServerSettings::GetExpireAfterSeconds() const
{
	return ExpireAfterSeconds;
}

