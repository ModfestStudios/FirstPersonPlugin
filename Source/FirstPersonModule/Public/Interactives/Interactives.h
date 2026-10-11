
#pragma once
#include "CoreMinimal.h"
#include "Interactives.generated.h"


UENUM(BlueprintType)
enum class EInteractiveShape : uint8
{
	Box,
	Sphere,
	Capsule
};

UENUM(BlueprintType)
enum class EInteractionFunctionType : uint8
{
	NativeEvent,
	CustomFunction
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	//instantly complete the action
	Instant,
	//time-based trigger
	Duration,
	//begins interaction and waits to be canceled/completed externally
	Manual
};

/*enum used when defining WHY an interaction ended*/
UENUM(BlueprintType)
enum class EInteractionEndReason : uint8
{
	Completed, //The action was succesfully completed
	Canceled, //The user canceled before the action completed
	Interrupted //the user was interrupted before the action completed
};


USTRUCT(BlueprintType)
struct FInteractiveActionConfig
{
	GENERATED_BODY()
public:

	UPROPERTY()
		FGuid ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool bEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		TSubclassOf<UInteractiveAction> Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
		bool bOverrideActionLabel = false;
		/*if filled - will be used instead of the UInteractiveActions default label*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "bOverrideActionLabel"))
		FText LabelOverride;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		EInteractionType InteractionType = EInteractionType::Instant;
	/*how long the action takes to complete (player must hold the interaction button down for this amount of time before completion is triggered)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "InteractionType == EInteractionType::Duration"))
		float ActionDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		EInteractionFunctionType FunctionType = EInteractionFunctionType::NativeEvent;
	/*if filled - will automatically call the blueprint or C++ with matching function name on the owning actor when activated. Note: Use FInteractionEventParams as a parameter of the function to get references to Action and InteractiveCollisionComponent passed in. MUST BE A UFUNCTION() for C++ to work*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "FunctionType == EInteractionFunctionType::CustomFunction"))
		FName FunctionToCall = NAME_None;

	FInteractiveActionConfig()
	{
		if(!ID.IsValid())
			ID = FGuid::NewGuid();
	}
	FInteractiveActionConfig(TSubclassOf<UInteractiveAction> Action)
	{

		if (!ID.IsValid())
			ID = FGuid::NewGuid();

		this->Action = Action;
		this->InteractionType = EInteractionType::Instant;
		this->ActionDuration = 0.0f;
		this->bEnabled = true;
		this->bOverrideActionLabel = false;
		this->LabelOverride = FText::FromString("");
		this->FunctionType = EInteractionFunctionType::NativeEvent;
		this->FunctionToCall = NAME_None;
	}
	FInteractiveActionConfig(TSubclassOf<UInteractiveAction> Action, float ActionDuration)
	{
		if (!ID.IsValid())
			ID = FGuid::NewGuid();

		this->Action = Action;
		this->InteractionType = EInteractionType::Duration;
		this->ActionDuration = ActionDuration;
		bEnabled = true;
		bOverrideActionLabel = false;
		LabelOverride = FText::FromString("");
		FunctionToCall = NAME_None;
	}
};

USTRUCT(BlueprintType)
struct FInteractionEventParams
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		const UInteractiveAction* Action;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		const class AFirstPersonCharacter* User;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		class UInteractiveComponent* InteractiveComponent;

	FInteractionEventParams()
		: Action(nullptr), User(nullptr), InteractiveComponent(nullptr)
	{
	}
};

USTRUCT(BlueprintType)
struct FInteractiveGridEntry
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TSet<TWeakObjectPtr<UInteractiveComponent>> Interactives;
};

USTRUCT(BlueprintType)
struct FInteractiveVicinityCheckParameters
{
	GENERATED_BODY()

public:

	/*Ignore InteractiveComponents owned by these actors*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		TArray<AActor*> IgnoredActors;

		/*Measure Threshold using XY distance only. MaxVerticalDistance can still restrict height.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool bUse2DDistance = false;

		/*Only return InteractiveComponents whose interactions are currently enabled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool bOnlyEnabledInteractives = true;

		/*Maximum allowed vertical separation. <= 0 disables the vertical restriction.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
		float MaxVerticalDistance = 180.0f;

		/*Require an unobstructed Visibility trace between the query location and the InteractiveComponent*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool bPerformVisibilitySweep = false;
};







DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionAction, class AFirstPersonCharacter*, User, const class UInteractiveAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionHover, class AFirstPersonCharacter*, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteraction, AFirstPersonCharacter*, User, const UInteractiveAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionEnded, AFirstPersonCharacter*, User, const UInteractiveAction*, Action, EInteractionEndReason, EndReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractiveHoverChange, AFirstPersonCharacter*, User, bool, bHovered);