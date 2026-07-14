// Copyrighted : Modfest Studios 2025-2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateList.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UPlayerStateList : public UUserWidget
{
	GENERATED_BODY()
public:

	FTimerHandle BindRetryHandle;


	//===================================================================================================================================================================
	//=============================================================================FUNCTIONS=============================================================================
	//===================================================================================================================================================================

	
	virtual void NativeConstruct() override;
	

	virtual void InitializeBindings();
	UFUNCTION()
		virtual void OnPlayerListInitialized();
	UFUNCTION(BlueprintImplementableEvent, Category = "Player States", meta = (DisplayName = "On Player List Initialized"))
		void BP_OnPlayerListInitialized();

	
	UFUNCTION()
		virtual void OnPlayerAssignedRosterChanged(class AFirstPersonPlayerState* PlayerState, class ARosterInfo* AssignedRoster);
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State", meta = (DisplayName = "On Player Assigned Roster Changed"))
		void BP_OnPlayerAssignedRosterChanged(class AFirstPersonPlayerState* PlayerState, class ARosterInfo* AssignedRoster);
	UFUNCTION()
		virtual void OnPlayerReadyStateChanged(class AFirstPersonPlayerState* PlayerState, bool bIsReady);
	UFUNCTION(BlueprintImplementableEvent, Category = "Player States", meta = (DisplayName="On Player Ready State Changed"))
		void BP_OnPlayerReadyStateChanged(class AFirstPersonPlayerState* PlayerState, bool bIsReady);

	UFUNCTION()
		virtual void OnPlayerJoins(class AFirstPersonPlayerState* PlayerState);
	UFUNCTION(BlueprintImplementableEvent, Category = "Player States", meta = (DisplayName = "On Player Joins"))
		void BP_OnPlayerJoins(class AFirstPersonPlayerState* PlayerState);
	


	UFUNCTION()
		virtual void OnPlayerLeaves(class AFirstPersonPlayerState* PlayerState);
	UFUNCTION(BlueprintImplementableEvent, Category = "Player States", meta = (DisplayName = "On Player Leaves"))
		void BP_OnPlayerLeaves(class AFirstPersonPlayerState* PlayerState);



	virtual void NativeDestruct() override;

};
