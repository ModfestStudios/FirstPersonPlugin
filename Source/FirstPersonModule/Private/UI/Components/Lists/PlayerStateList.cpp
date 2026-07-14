// Copyrighted : Modfest Studios 2025-2026


#include "UI/Components/Lists/PlayerStateList.h"
#include "Players/FirstPersonPlayerState.h"

/*gamemodes*/
#include "GameModes/FirstPersonGameState.h"


void UPlayerStateList::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeBindings();	
}


/*InitializeBindings() - GameState can take some time for a client to retrieve fully
* so we use this function which will call itself if it finds GS is not ready yet to ensure proper binding
*
*/
void UPlayerStateList::InitializeBindings()
{
	if (AFirstPersonGameState* GS = GetWorld()->GetGameState<AFirstPersonGameState>())
	{
		GS->OnPlayerAssignedRosterChanged.AddDynamic(this,&UPlayerStateList::OnPlayerAssignedRosterChanged);
		GS->OnPlayerReadyStateChanged.AddDynamic(this, &UPlayerStateList::OnPlayerReadyStateChanged);
		GS->OnPlayerJoins.AddDynamic(this, &UPlayerStateList::OnPlayerJoins);
		GS->OnPlayerLeaves.AddDynamic(this, &UPlayerStateList::OnPlayerLeaves);

		OnPlayerListInitialized();

		/*clear re-try attemps*/
		if(GetWorld()->GetTimerManager().IsTimerActive(BindRetryHandle))
			GetWorld()->GetTimerManager().ClearTimer(BindRetryHandle);
	}

	/*gamestate is not ready - so try again shortly*/
	else
	{
		GetWorld()->GetTimerManager().SetTimer(BindRetryHandle, this,&UPlayerStateList::InitializeBindings,0.1f,false);
	}
}

void UPlayerStateList::OnPlayerListInitialized()
{

	BP_OnPlayerListInitialized();
}

void UPlayerStateList::OnPlayerAssignedRosterChanged(AFirstPersonPlayerState* PlayerState, ARosterInfo* AssignedRoster)
{
	BP_OnPlayerAssignedRosterChanged(PlayerState, AssignedRoster);
}

void UPlayerStateList::OnPlayerReadyStateChanged(AFirstPersonPlayerState* PlayerState, bool bIsReady)
{
	

	BP_OnPlayerReadyStateChanged(PlayerState,bIsReady);
}

void UPlayerStateList::OnPlayerJoins(AFirstPersonPlayerState* PlayerState)
{

	BP_OnPlayerJoins(PlayerState);
}

void UPlayerStateList::OnPlayerLeaves(AFirstPersonPlayerState* PlayerState)
{

	BP_OnPlayerLeaves(PlayerState);
}

void UPlayerStateList::NativeDestruct()
{	
	if (AFirstPersonGameState* GS = GetWorld()->GetGameState<AFirstPersonGameState>())
	{
		GS->OnPlayerAssignedRosterChanged.RemoveDynamic(this,&UPlayerStateList::OnPlayerAssignedRosterChanged);
		GS->OnPlayerReadyStateChanged.RemoveDynamic(this, &UPlayerStateList::BP_OnPlayerReadyStateChanged);
		GS->OnPlayerJoins.RemoveDynamic(this, &UPlayerStateList::OnPlayerJoins);
		GS->OnPlayerLeaves.RemoveDynamic(this, &UPlayerStateList::OnPlayerLeaves);
	}
	
	Super::NativeDestruct();
}
