// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InteractiveManagerComponent.h"
#include "InteractiveComponent.generated.h"


UCLASS(ClassGroup = ("Interactives"), meta = (BlueprintSpawnableComponent))
class FIRSTPERSONMODULE_API UInteractiveComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:


	UPROPERTY(EditDefaultsOnly, Category = "Material")
		bool bHighlightOnHover = false;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		FName HighlightMaterialParameterName = "Highlight Intensity";
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float HighlightMaterialIntensity = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Material")
		float DefaultMaterialIntensity = 0.0f;

	UPROPERTY()
		UMaterialInstanceDynamic* DynamicMaterial = nullptr;



	/*if enabled - will automatically find the UFUNCTION() (required) in the owner's C++ code and call it*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
		bool bAutoCallFunction = false;
	/*name of the UFUNCTION() to call in C++ of this component's owner*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction",meta = (EditCondition=bAutoCallFunction))
		FName FunctionToCall;


	UPROPERTY(BlueprintAssignable)
		FOnInteraction OnInteraction;



	UPROPERTY(EditAnywhere, Category = "Animation")
		UCurveFloat* AnimationCurve;


	//===================================================================================================================================
	//=============================================================FUNCTIONS=============================================================
	//===================================================================================================================================

public:	
	// Sets default values for this component's properties
	UInteractiveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/*public function that can be called that triggers interaction of this component and will call events OnInteraction() as well as any custom defined function on the owner using FunctionToCall*/
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		virtual void Interact(class AFirstPersonCharacter* User);
protected:
	UFUNCTION()
		virtual void NativeOnInteraction(class AFirstPersonCharacter* User);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "OnInteraction"))
		void BP_OnInteraction(class AFirstPersonCharacter* User);


protected:
	/*will find and call a UFUNCTION on the owner that matches the FunctionToCall name*/
	UFUNCTION()
		void CallOwnerFunctionByName();


	//============================
	//======ELEVATOR BUTTONS======
	//============================
public:
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void OnHovered(class AFirstPersonCharacter* User);
	UFUNCTION(BlueprintCallable, Category = "Interactions")
		virtual void OnUnhovered(class AFirstPersonCharacter* User);
	//UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta = (DisplayName = "On Button Hovered"))
	//	void BP_OnButtonHovered(class AFirstPersonCharacter* User);
	//UFUNCTION(BlueprintImplementableEvent, Category = "Interactions", meta = (DisplayName = "On Button Unhovered"))
	//	void BP_OnButtonUnhovered(class AFirstPersonCharacter* User);
		
};
