// Copyrighted Modfest Studios 2024

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/FirstPersonCharacter.h"
#include "ThirdPersonAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UThirdPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UPROPERTY()
		class AFirstPersonCharacter* Character;

	/*movement*/
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (DisplayName = "Moving"));
		bool PF_bMoving;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float MovementSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
		FVector2D MovementDirection;

	/*death*/
	UPROPERTY(EditDefaultsOnly, Category = "Health|Death")
		bool bDying = false;

	//=========================================================================================================================
	//========================================================FUNCTIONS========================================================
	//=========================================================================================================================

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void NativeThreadSafeUpdateAnimation(float DeltaTime) override;
	


	//==============================
	//===========MOVEMENT===========
	//==============================
private:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Movement", meta = (BlueprintThreadSafe))
		bool IsMoving();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement", meta=(BlueprintThreadSafe))
		float GetMovementSpeed();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement", meta = (BlueprintThreadSafe))
		float GetForwardMovementDirection();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement", meta = (BlueprintThreadSafe))
		float GetSidewayMovementDirection();



	//=============================
	//============DEATH============
	//=============================
private:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Health|Death", meta = (BlueprintThreadSafe))
		bool IsDying();


public:
	/*event called when the player dies*/
	UFUNCTION()
		virtual void OnDeath(const FDeathInfo DeathInfo);

	

};
