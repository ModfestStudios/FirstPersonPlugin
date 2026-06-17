// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryAttachmentCompEntry.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UInventoryAttachmentCompEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:

	/*if enabled - will search the owning pawn for matching equipment slot*/
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Auto Initialize")
		bool bAutoInitialize = false;
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Auto Initialize", meta = (EditCondition = bAutoInitialize))
		FName AttachmentID = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Auto Initialize", meta = (EditCondition = bAutoInitialize, AllowAbstract = "true"))
		TSubclassOf<class AInventoryItem> ItemType;


	UPROPERTY(BlueprintReadOnly, Category = "Inventory Attachment")
		class UInventoryAttachmentComponent* AttachmentComponent;


	//===========================================================================================================================================================================
	//=================================================================================FUNCTIONS=================================================================================
	//===========================================================================================================================================================================

	virtual void NativeOnInitialized() override;


	/*object is passed in - should be converted to UMapInfo and setse MapInfo*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;



	UFUNCTION(BlueprintCallable, Category = "Inventory Attachment")
		virtual void SetAttachmentComponent(class UInventoryAttachmentComponent* NewAttachment);
	UFUNCTION()
		virtual void OnAttachmentComponentChanged();
	UFUNCTION(BlueprintImplementableEvent, category = "Inventory Attachment", meta = (DisplayName = "OnAttachmentComponentChanged"))
		void BP_OnAttachmentComponentChanged();
};
