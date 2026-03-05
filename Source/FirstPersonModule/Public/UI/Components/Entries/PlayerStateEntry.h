// copyrighted 2024-2025 - Modfest Studios & Micah A. Parker

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PlayerStateEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UPlayerStateEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadOnly, Category = "Player State")
		class AFirstPersonPlayerState* PlayerState;
	UPROPERTY(BlueprintReadOnly, Category = "Player State")
		bool bPlayerReady = false;



	//=====================================================================================================================================
	//==============================================================FUNCTIONS==============================================================
	//=====================================================================================================================================


		/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	/*selection state*/
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;


	UFUNCTION()
		virtual void NativeOnReadyStateChanged(class AFirstPersonPlayerState* Player, bool bIsReady);
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State", meta=(DisplayName="On Ready State Changed"))
		void BP_OnReadyStateChanged(class AFirstPersonPlayerState* Player, bool bIsReady);


	virtual void NativeDestruct() override;
	
};
