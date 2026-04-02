// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "FirstPersonLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API AFirstPersonLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
public:




	//=============================================================================================================================
	//==========================================================FUNCTIONS==========================================================
	//=============================================================================================================================

	
	//=================
	//=====PLAYERS=====
	//=================
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Players")
		void OnPlayerDeath(class AFirstPersonCharacter* Character, AActor* DamageCauser, AController* DamageInstigator, const UDamageType* DamageType);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Players")
		TArray<class AFirstPersonPlayerController*> GetAllPlayers();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Players")
		bool AllPlayersDead();

	//==================
	//=======MAPS=======
	//==================
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Maps")
		void OnCurrentMapChanged(class UMapAsset* NewMap);
	UFUNCTION(BlueprintImplementableEvent, Category = "Maps")
		void OnNextMapChanged(class UMapAsset* NextMap);
	UFUNCTION(BlueprintImplementableEvent, Category = "Maps")
		void OnMapRotationChanged(const TArray<class UMapAsset*>& NewRotation);

	//==================
	//=====MISSIONS=====
	//==================
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Missions")
		void OnMissionStart();

	
};
