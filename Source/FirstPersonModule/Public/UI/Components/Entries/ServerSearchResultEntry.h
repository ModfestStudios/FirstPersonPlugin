// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Online/ServerBrowserSearchResult.h"
#include "ServerSearchResultEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UServerSearchResultEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Server Search Results")
		UServerBrowserSearchResult* SearchResult;

	/*object is passed in - should be converted to UServerBrowserSearchResult and sets SearchResult*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	
};
