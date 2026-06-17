// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryAttachmentComponent.generated.h"


UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONMODULE_API UInventoryAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/*when set - will automatically spawn && attach this item at start*/
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment")
		TSubclassOf<class AInventoryItem> DefaultAttachment;

	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Setup")
		FText AttachmentName;
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Setup")
		FName AttachmentID;
	/*set the parent class to ensure this attachment only accepts certain types*/
	UPROPERTY(EditAnywhere, Category = "Inventory Attachment|Setup", meta = (AllowAbstract = "true"))
		TSubclassOf<class AInventoryItem> AttachmentType;



protected:
	/*the instanced item attached to us*/
	UPROPERTY()
		class AInventoryItem* AttachedItem;
	//=======================================================================================================================
	//=======================================================FUNCTIONS=======================================================
	//=======================================================================================================================


public:	
	// Sets default values for this component's properties
	UInventoryAttachmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Attachment")
		class AInventoryItem* GetAttachedItem();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Attachment")
		class UInventoryManagerComponent* GetAttachedInventoryManager();
	/*spawns and attaches the default attachment to this equipment slot*/
	UFUNCTION()
		virtual void SpawnDefaultAttachment();


		
};
