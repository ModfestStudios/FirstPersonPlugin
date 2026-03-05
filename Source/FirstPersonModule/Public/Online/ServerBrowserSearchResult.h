// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OnlineSessionSettings.h"

#include "ServerBrowserSearchResult.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UServerBrowserSearchResult : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Server Info")
		FString ServerName;
	UPROPERTY(BlueprintReadOnly, Category = "Server Info")
		int32 Ping;
	UPROPERTY(BlueprintReadOnly, Category = "Server Info")
		int32 CurrentPlayers;
	UPROPERTY(BlueprintReadOnly, Category = "Server Info")
		int32 MaxPlayers;

	FOnlineSessionSearchResult SearchResult;
	void InitOnlineResult(const FOnlineSessionSearchResult& Result);
	
};
