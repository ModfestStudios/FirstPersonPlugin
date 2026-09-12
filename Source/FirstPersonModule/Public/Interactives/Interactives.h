
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
	ComponentEvent,
	CustomFunction
};

USTRUCT(BlueprintType)
struct FInteractiveActionCall
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		TSubclassOf<UInteractiveAction> Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		EInteractionFunctionType FunctionType = EInteractionFunctionType::ComponentEvent;
	/*if filled - will automatically call the blueprint or C++ with matching function name on the owning actor when activated. Note: Use FInteractionEventParams as a parameter of the function to get references to Action and InteractiveCollisionComponent passed in. MUST BE A UFUNCTION() for C++ to work*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "FunctionType == EInteractionFunctionType::CustomFunction"))
		FName FunctionToCall;
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



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionAction, class AFirstPersonCharacter*, User, const class UInteractiveAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionHover, class AFirstPersonCharacter*, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteraction, AFirstPersonCharacter*, User, const UInteractiveAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionEnded, AFirstPersonCharacter*, User, const UInteractiveAction*, Action, EInteractionEndReason, EndReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractiveHoverChange, AFirstPersonCharacter*, User, bool, bHovered);