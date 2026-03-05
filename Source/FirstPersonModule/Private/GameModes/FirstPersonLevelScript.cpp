// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker


#include "GameModes/FirstPersonLevelScript.h"
#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"
#include "Characters/FirstPersonCharacter.h"
#include "Kismet/GameplayStatics.h"




TArray<class AFirstPersonPlayerController*> AFirstPersonLevelScript::GetAllPlayers()
{
	TArray<AFirstPersonPlayerController*> PlayerControllers;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFirstPersonPlayerController* PC = Cast<AFirstPersonPlayerController>(Iterator->Get());

		if (IsValid(PC))
			PlayerControllers.AddUnique(PC);
	}

	return PlayerControllers;
}

bool AFirstPersonLevelScript::AllPlayersDead()
{
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFirstPersonCharacter::StaticClass(), Characters);

	for(int32 i = 0; i < Characters.Num(); i++)
	{
		AFirstPersonCharacter* Char = Cast<AFirstPersonCharacter>(Characters[i]);
		if (Char->IsPlayerControlled() == true && Char->IsPlayerDead() == false)
			return false;
	}

	return true;
}
