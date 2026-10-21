// Copyright (c) Modfest Studios
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Inventory.h"
#include "InventoryAttachmentComponent.generated.h"

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UInventoryAttachmentComponent : public UActorComponent
{
    GENERATED_BODY()
    friend class UInventoryManagerComponent;
public:
    UInventoryAttachmentComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="Inventory Attachment")
    FInventoryEquipmentSlotConfig Config;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="Inventory Attachment")
    class UInventoryManagerComponent* InventoryManager = nullptr;

    UFUNCTION(BlueprintPure, Category="Inventory Attachment")
    class AInventoryItem* GetAttachedItem() const;
    UFUNCTION(BlueprintPure, Category="Inventory Attachment")
    class UInventoryManagerComponent* GetAttachedInventoryManager() const;
    UFUNCTION(BlueprintPure, Category="Inventory Attachment")
    bool IsItemEquipped() const;
    UFUNCTION(BlueprintPure, Category="Inventory Attachment")
    bool CanAcceptItem(const class AInventoryItem* Item) const;
    UFUNCTION(BlueprintCallable, Category="Inventory Attachment")
    void ApplyAttachment();
    UFUNCTION(BlueprintPure, Category="Inventory Attachment")
    class USceneComponent* GetAttachmentTarget() const;

protected:
    UPROPERTY(ReplicatedUsing=OnRep_AttachedItem)
    class AInventoryItem* AttachedItem = nullptr;
    UFUNCTION()
    void OnRep_AttachedItem();
};
