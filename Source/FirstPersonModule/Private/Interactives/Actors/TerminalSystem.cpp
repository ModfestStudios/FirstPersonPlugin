// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/TerminalSystem.h"

/*components*/
#include "Components/WidgetComponent.h"
#include "Components/InteractiveCollisionComponent.h"

/*camera*/
#include "Camera/CameraComponent.h"

/*characters*/
#include "Players/FirstPersonPlayerController.h"
#include "Characters/FirstPersonCharacter.h"

/*materials*/
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget.h"
#include "Engine/TextureRenderTarget2D.h"


/*operating system*/
#include "OperatingSystem/OperatingSystem.h"
#include "UI/Widgets/OperatingSystemWidget.h"


/*utilities*/
#include "TimerManager.h"


// Sets default values
ATerminalSystem::ATerminalSystem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("CameraComponent"));
	if (Camera)
	{
		Camera->SetupAttachment(Mesh);
		Camera->SetRelativeLocation(FVector(90.0f, 0.0f, 52.0f));
		Camera->SetRelativeRotation(FRotator(-5, 180, 0));
		Camera->SetFieldOfView(100.0f);
	}

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(GetRootComponent());
		WidgetComponent->bRenderInMainPass = false; //disables widget from rendering in-world as a floating plane, so that only the render target works
	}

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATerminalSystem::BeginPlay()
{
	Super::BeginPlay();

	if (bBootOnBeginPlay)
	{
		StartupTerminal();		
	}
}

// Called every frame
void ATerminalSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ATerminalSystem::StartupTerminal()
{
	/*servers*/
	if (GetNetMode() < NM_Client)
	{
		/*no operating system - we'll need to create one and begin boot*/
		if (!OperatingSystem)
		{
			if (OperatingSystemClass)
			{
				OperatingSystem = GetWorld()->SpawnActor<AOperatingSystem>(OperatingSystemClass);

				
				if (OperatingSystem)
					OperatingSystem->InitializeFromTerminal(this);
			}
		}

		InitializeTerminalWidgetDisplay();

		/*skips the boot sequences if it's meant to be loaded at game start*/
		if (bBootOnBeginPlay)
		{
			
		}
		/**/
		else
		{
			BootOperatingSystem();
		}
		
	}	
}

void ATerminalSystem::BootOperatingSystem()
{
	/*server only*/
	if (GetNetMode() < NM_Client)
	{
		if (OperatingSystem)
		{
			OperatingSystem->Boot();
		}
	}	
}

void ATerminalSystem::ShutdownTerminal()
{

}

void ATerminalSystem::NativeOnInteractionBegins(AFirstPersonCharacter* User, const UInteractiveAction* Action)
{
	Super::NativeOnInteractionBegins(User, Action);

	EnterTerminal(User);	
}

void ATerminalSystem::EnterTerminal(AFirstPersonCharacter* User)
{
	if (AFirstPersonPlayerController* PC = User->GetController<AFirstPersonPlayerController>())
	{
		PC->SetInputMode(FInputModeUIOnly::FInputModeUIOnly());

		/*animate camera towards terminal*/
		SetPlayerViewToCamera(PC);		

		/*create and pullup the widget for the player to interact with*/
		GetWorld()->GetTimerManager().SetTimer(
			WidgetAddHandle,
			FTimerDelegate::CreateLambda([this, PC]()
		{
			AddWidgetToViewport(PC);
		}),
			CameraBlendTime + .30f,
			false
		);
	}
}

void ATerminalSystem::ExitTerminal(AFirstPersonCharacter* User)
{
	if(AFirstPersonPlayerController* PC = User->GetController<AFirstPersonPlayerController>())
	{
		RemoveWidgetFromViewport(PC);
		RestorePlayerView(PC);
	}
}


void ATerminalSystem::SetPlayerViewToCamera(AFirstPersonPlayerController* Player)
{
	if (Player)
		Player->SetViewTargetWithBlend(this, CameraBlendTime, EViewTargetBlendFunction::VTBlend_EaseIn);
}

void ATerminalSystem::RestorePlayerView(AFirstPersonPlayerController* Player)
{
	if (Player && Player->GetViewTarget() == this)
		Player->SetViewTargetWithBlend(Player->GetPawn(), CameraBlendTime, EViewTargetBlendFunction::VTBlend_EaseIn);		
}



//==================
//========UI========
//==================
UOperatingSystemWidget* ATerminalSystem::GetTerminalWidget()
{
	return Cast<UOperatingSystemWidget>(WidgetComponent->GetWidget());
}

UOperatingSystemWidget* ATerminalSystem::GetViewportWidget()
{
	return ViewportWidget;
}


void ATerminalSystem::InitializeTerminalWidgetDisplay()
{
	/*create the material instance we can use during gameplay*/
	if (Mesh != nullptr && ScreenMaterial != nullptr && DynamicScreenMaterial == nullptr)
	{
		/*create material*/
		FName MaterialName = FName(GetFName().ToString() + "_ScreenMaterialInstance");
		DynamicScreenMaterial = UMaterialInstanceDynamic::Create(ScreenMaterial, this, MaterialName);
		
		/*apply to mesh*/
		Mesh->SetMaterialByName(ScreenMaterialSlotName, DynamicScreenMaterial);

		/*initial refresh of the material*/
		GetWorld()->GetTimerManager().SetTimer(WidgetRefreshHandle, this, &ATerminalSystem::RefreshRenderTarget, 1.0f); //delay needed for some reason to make it work outside of tick
	}
}

void ATerminalSystem::RefreshRenderTarget()
{
	if (DynamicScreenMaterial)
	{
		/*get the render target created automatically from the widget*/
		UTextureRenderTarget2D* RenderTarget = WidgetComponent->GetRenderTarget();

		/*update the mesh material with the render*/
		DynamicScreenMaterial->SetTextureParameterValue(RenderTargetParameterName, RenderTarget);
	}
}

void ATerminalSystem::AddWidgetToViewport(AFirstPersonPlayerController* Player)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(WidgetAddHandle))
		GetWorld()->GetTimerManager().ClearTimer(WidgetAddHandle);

	/*safety check*/
	if(!Player)
		return;

	if (!GetViewportWidget())
	{
		UClass* WidgetClass = WidgetComponent->GetWidgetClass();

		if (!WidgetClass || !WidgetClass->IsChildOf(UOperatingSystemWidget::StaticClass()))
		{
			UE_LOG(LogTemp,Warning,TEXT("!!!%s::AddWidgetToViewport() - WIDGET COMPONENT WIDGET CLASS IS MISSING OR NOT EXTENDING UOperatingSystemWidget!!!"), *GetName());
			return;
		}

		ViewportWidget = CreateWidget<UOperatingSystemWidget>(Player,WidgetClass, FName("OperatingSystemWidget"));
		
		if(GetViewportWidget())
			GetViewportWidget()->InitializeWidget(OperatingSystem);
	}

	
	if (GetViewportWidget())
	{
		GetViewportWidget()->AddToViewport(10);

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(ViewportWidget->TakeWidget());		
		Player->SetInputMode(InputMode);
		GetViewportWidget()->SetKeyboardFocus();		
		//GetViewportWidget()->SetFocus();
		Player->bShowMouseCursor = true;
	}
}

void ATerminalSystem::RemoveWidgetFromViewport(AFirstPersonPlayerController* Player)
{
	if(!Player)
		return;

	if (GetViewportWidget())
	{
		GetViewportWidget()->RemoveFromParent();
	}	

	Player->SetInputMode(FInputModeGameOnly());
	Player->bShowMouseCursor = false;
}

