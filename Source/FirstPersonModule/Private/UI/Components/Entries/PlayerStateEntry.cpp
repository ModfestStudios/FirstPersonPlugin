// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "UI/Components/Entries/PlayerStateEntry.h"
#include "Players/FirstPersonPlayerState.h"

void UPlayerStateEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	
	AFirstPersonPlayerState* NewPlayerState = Cast<AFirstPersonPlayerState>(ListItemObject);

	/*clear old bindings*/
	if (PlayerState != nullptr && PlayerState != NewPlayerState)
		PlayerState->OnReadyStateChanged.RemoveDynamic(this, &UPlayerStateEntry::NativeOnReadyStateChanged);

	PlayerState = NewPlayerState;
	PlayerState->OnReadyStateChanged.AddDynamic(this, &UPlayerStateEntry::NativeOnReadyStateChanged);

	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UPlayerStateEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{

}

void UPlayerStateEntry::NativeOnReadyStateChanged(AFirstPersonPlayerState* Player, bool bIsReady)
{
	bPlayerReady = bIsReady;

	/*notify blueprint event*/
	BP_OnReadyStateChanged(Player, bIsReady);
}

void UPlayerStateEntry::NativeDestruct()
{
	if (PlayerState)
		PlayerState->OnReadyStateChanged.RemoveDynamic(this, &UPlayerStateEntry::NativeOnReadyStateChanged);

}
