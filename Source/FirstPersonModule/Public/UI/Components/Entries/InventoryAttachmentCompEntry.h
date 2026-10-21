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
		class UInventoryAttachmentComponent* AttachmentComponent = nullptr;


	//===========================================================================================================================================================================
	//=================================================================================FUNCTIONS=================================================================================
	//===========================================================================================================================================================================

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	/*assigns the runtime attachment component represented by this list entry*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;



	/*retries automatic binding when the owning pawn or runtime slots become available*/
	UFUNCTION(BlueprintCallable, Category = "Inventory Attachment")
		bool RefreshAttachmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory Attachment")
		virtual void SetAttachmentComponent(class UInventoryAttachmentComponent* NewAttachment);
	UFUNCTION(BlueprintPure, Category = "Inventory Attachment")
		bool HasValidAttachedItem() const;

	UFUNCTION()
		virtual void OnAttachmentComponentChanged();
	UFUNCTION(BlueprintImplementableEvent, category = "Inventory Attachment", meta = (DisplayName = "OnAttachmentComponentChanged"))
		void BP_OnAttachmentComponentChanged();
private:
	bool bUsesListItemBinding = false;
};
