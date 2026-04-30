// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactives/InteractiveActor.h"
#include "TerminalSystem.generated.h"

UCLASS()
class FIRSTPERSONMODULE_API ATerminalSystem : public AInteractiveActor
{
	GENERATED_BODY()
public:

	/*system will automatically be booted at start of game - rather than requiring it to be booted*/
	UPROPERTY(EditAnywhere, Category = "Terminal System|Operating System")
		bool bBootOnBeginPlay;


protected:
	/*the operating system to create upon booting*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Terminal|Operating System")
		TSubclassOf<class AOperatingSystem> OperatingSystemClass;
	/*operating system*/
	UPROPERTY()
		class AOperatingSystem* OperatingSystem;

protected:
	/*the name of the material slot of the static mesh that we wish to replace with the render target version*/
	UPROPERTY(EditAnywhere, Category = "Terminal")
		FName ScreenMaterialSlotName = "Screen";
	/*the material we wish to use for the screen*/
	UPROPERTY(EditAnywhere, Category = "Terminal")
		class UMaterialInterface* ScreenMaterial;

	/*the created material on BeginPlay that lives on the instance*/
	UPROPERTY(BlueprintReadWrite, Category = "Terminal")
		class UMaterialInstanceDynamic* DynamicScreenMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terminal")
		FName RenderTargetParameterName = "RenderTarget";


	/*camera*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terminal|Camera")
		float CameraBlendTime = 0.45f;

	/*user-widget*/
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terminal|User Interface")
	//	TSubclassOf<class UOperatingSystemWidget> OperatingSystemWidgetClass;
	
	FTimerHandle WidgetAddHandle;
	FTimerHandle WidgetRefreshHandle;

public:
	/*components*/
	UPROPERTY()
		class UCameraComponent* Camera;
	/*the widget component that controls/renders the UMG portion*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UWidgetComponent* WidgetComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rendering")
		class UInteractiveCollisionComponent* InteractiveCollision;
// 
	//===================================================================================================================================================================================================
	//=============================================================================================FUNCTIONS=============================================================================================
	//===================================================================================================================================================================================================

	
public:	
	// Sets default values for this actor's properties
	ATerminalSystem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void StartupTerminal();
protected:
	UFUNCTION()
		virtual void BootOperatingSystem();

public:
	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void ShutdownTerminal();


	//=====================
	//=====INTERACTION=====
	//=====================

	virtual void OnInteraction(class AFirstPersonCharacter* User, class UInteractiveCollisionComponent* InteractiveComponent, const class UInteractiveAction* Action) override;

	//================
	//=====CAMERA=====
	//================
	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void SetPlayerViewToCamera(class AFirstPersonPlayerController* Player);
	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void RestorePlayerView(class AFirstPersonPlayerController* Player);




	//==================
	//========UI========
	//==================
public:
	UFUNCTION(BlueprintPure, Category = "Terminal System|UI")
		class UOperatingSystemWidget* GetOperatingSystemWidget();
protected:
	/*initializes the Widget Component and creates a Dynamic Material to render it onto the mesh with*/
	UFUNCTION()
		void InitializeWidgetDisplay();
	UFUNCTION()
		void RefreshRenderTarget();


	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void AddWidgetToViewport(class AFirstPersonPlayerController* Player);
	UFUNCTION(BlueprintCallable, Category = "Terminal System")
		virtual void RemoveWidgetFromViewport(class AFirstPersonPlayerController* Player);

};
