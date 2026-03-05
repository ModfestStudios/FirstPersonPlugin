// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/UnrealType.h"
#include "CollisionShape.h"

#include "Engine/CollisionProfile.h"
#include "Characters/FirstPersonCharacter.h"
#include "InteractiveManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteraction, AFirstPersonCharacter*, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractiveHoverChange, AFirstPersonCharacter*, User, bool, bHovered);

UENUM()
enum class EInteractiveTraceType : uint8
{
	Controller,
	Camera
};

UENUM()
enum class EInteractionStatus : uint8
{
	Unknown,
	Completed,
	Canceled,
	Failed
};

USTRUCT()
struct FInteraction
{
	GENERATED_BODY()
public:

	UPROPERTY()
		UActorComponent* Interactive = nullptr;
	UPROPERTY()
		EInteractionStatus Status;


	FInteraction() {};

};



UCLASS(ClassGroup = (Interactions), meta = (BlueprintSpawnableComponent), meta=(RestrictedToClasses="AFirstPersonCharacter"))
class FIRSTPERSONMODULE_API UInteractiveManagerComponent : public UActorComponent
{
	friend class UInteractiveCollisionComponent;

	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		TEnumAsByte<ETraceTypeQuery> TraceChannel;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		EInteractiveTraceType TraceType;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		float TraceLength;
	UPROPERTY(EditDefaultsOnly, Category = "Interactions")
		float ProbeSize = 2.0f;



	UPROPERTY(EditDefaultsOnly, Category = "Interactions|Debug")
		bool bDebugTrace = false;

	class UCameraComponent* PlayerCamera;


private:
	/*all possible interactions near us*/
	UPROPERTY()
		TArray<class UInteractiveCollisionComponent*> Interactives;
	UPROPERTY()
		TArray<class UInteractiveCollisionComponent*> TracedInteractives;
	/*the current interaction we're using actively*/
	UPROPERTY()
		class UInteractiveCollisionComponent* ActiveInteraction;


	/*events/delegates*/
public:
	/*called whenever this Interactive Manager detects we're looking at an interactive item or move away from looking at it*/
	UPROPERTY(BlueprintAssignable)
		FOnInteractiveHoverChange OnInteractiveHoverStateChanged;
	UPROPERTY(BlueprintAssignable)
		FOnInteraction OnInteractionDenied;

	//=========================================================================================================
	//================================================FUNCTIONS================================================
	//=========================================================================================================

public:
	// Sets default values for this component's properties
	UInteractiveManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//================================================
	//==================INTERACTIONS==================
	//================================================
public:
	UFUNCTION()
		virtual void Interact();
	UFUNCTION()
		virtual void StopInteraction();
	UFUNCTION(BlueprintPure, Category = "Interactions")
		const TArray<FText> GetInteractiveActions();


protected:
	UFUNCTION()
		virtual bool BeginInteraction(class UInteractiveCollisionComponent* Interactive);
	/*client request to server to interact with object*/
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerRequestInteract(class UInteractiveCollisionComponent* Interactive);
	UFUNCTION()
		virtual void EndInteraction(class UInteractiveCollisionComponent* Interactive);

	//============================================
	//===========INTERACTIVES MANAGEMENT===========
	//============================================
protected:
	UFUNCTION()
		virtual void CalcInteractives();
	UFUNCTION()
		virtual bool IsInteractive(class UInteractiveCollisionComponent* Interactive);
	UFUNCTION()
		virtual void AddInteractive(class UInteractiveCollisionComponent*& Interactive);
	UFUNCTION()
		virtual void RemoveInteractive(class UInteractiveCollisionComponent*& Interactive);
	UFUNCTION()
		virtual void ClearInteractives();
	UFUNCTION()
		virtual void ClearActiveInteraction();
	UFUNCTION()
		virtual void UpdateActiveInteractiveStatus();
	UFUNCTION()
		virtual void NotifyInteractiveOfHover(class UInteractiveCollisionComponent*& Interactive);
	UFUNCTION()
		virtual void NotifyInteractiveOfUnhover(class UInteractiveCollisionComponent*& Interactive);
public:
	UFUNCTION()
		virtual void ReceiveDeniedInteraction(AActor* DeniedActor);
public:
	UFUNCTION(BlueprintPure, Category = "Interactives")
		UInteractiveCollisionComponent* GetCurrentInteractive();
	UFUNCTION(BlueprintPure, Category = "Interactives")
		AActor* GetCurrentInteractiveOwner();
	UFUNCTION(BlueprintPure, Category = "Interactives")
		virtual bool IsActiveInteractionComplete();
	/*returns true if we're currently trying to interact with something*/
	UFUNCTION(BlueprintPure, Category = "Interactives")
		virtual bool IsInteracting();


	//===============================
	//============TRACING============
	//===============================
protected:
	UFUNCTION()
		virtual void TraceForInteractives();
	UFUNCTION()
		virtual FVector GetTraceStart();
	UFUNCTION()
		virtual FVector GetTraceEnd();
	UFUNCTION()
		virtual FRotator GetTraceRotation();
	UFUNCTION()
		virtual ECollisionChannel GetTraceChannel();

	
	//===============================
	//===========UTILITIES===========
	//===============================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utilities")
		class AFirstPersonCharacter* GetCharacterOwner() const;
	
	class UCameraComponent* GetOwnerCamera();


};
