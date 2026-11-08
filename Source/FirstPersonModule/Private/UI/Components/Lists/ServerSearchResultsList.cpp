// Copyrighted : Modfest Studios 2025-2026


#include "UI/Components/Lists/ServerSearchResultsList.h"
#include "Subsystems/ServerSubsystem.h"

UServerSearchResultsList::UServerSearchResultsList(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UServerSearchResultsList::NativeConstruct()
{
	if(!GetGameInstance())
		return;

	if (bAutoBindToServerResults)
	{
		if (UServerSubsystem* SSS = GetGameInstance()->GetSubsystem<UServerSubsystem>())
		{
			SSS->OnSearchResultsUpdated.AddDynamic(this, &UServerSearchResultsList::NativeOnSearchResultsUpdated);
		}
	}	
}

void UServerSearchResultsList::NativeOnSearchResultsUpdated()
{
	if (UServerSubsystem* SSS = GetGameInstance()->GetSubsystem<UServerSubsystem>())
	{
		SearchResults = SSS->GetSearchResults();
	}
	
	BP_OnSearchResultsUpdated();
}
