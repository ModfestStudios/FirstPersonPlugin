// Fill out your copyright notice in the Description page of Project Settings.


#include "Players/FirstPersonPlayerController.h"
#include "Players/FirstPersonPlayerState.h"
#include "Characters/FirstPersonCharacter.h"
#include "Components/FirstPersonViewComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "GameModes/FirstPersonGame.h"
#include "GameFramework/SpectatorPawn.h"

/*engine*/
#include "Engine/World.h"
#include "Engine/GameInstance.h"

/*loadouts*/
#include "Inventory/InventoryLoadout.h"

/*input*/
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

/*subsystems*/
#include "Subsystems/CharacterSubsystem.h"
#include "Subsystems/GameMasterSubsystem.h"
#include "Subsystems/ServerSubsystem.h"

/*ui*/
#include "UMG.h"
#include "UI/Widgets/LoadoutWidget.h"

/*utilities*/
#include "UObject/ConstructorHelpers.h"

/*network*/
#include "Online.h"
#include "OnlineSubsystem.h"




AFirstPersonPlayerController::AFirstPersonPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ControllerMappingContextRef(TEXT("/FirstPersonModule/Blueprints/Input/Mappings/DefaultPlayerControllerMapping"));	

	if (ControllerMappingContextRef.Succeeded())
		ControllerMappingContext = ControllerMappingContextRef.Object;

	/*setup default actions*/
	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleServerInfoActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ToggleServerInfo"));
	if (ToggleServerInfoActionRef.Succeeded())
		ToggleServerInfoAction = ToggleServerInfoActionRef.Object;
	

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleEscapeMenuActionRef(TEXT("/FirstPersonModule/Blueprints/Input/Actions/ToggleEscapeMenu"));
	if (ToggleEscapeMenuActionRef.Succeeded())
		ToggleEscapeMenuAction = ToggleEscapeMenuActionRef.Object;
}

void AFirstPersonPlayerController::BeginPlay()
{
	if (GetWorld())
	{
		/*register self with GameMaster*/
		GetGameInstance()->GetSubsystem<UCharacterSubsystem>()->RegisterPlayerController(this);
	}
}

void AFirstPersonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/*add mapping context*/
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (InputSubsystem)
	{
		// Add the controller mapping context
		InputSubsystem->AddMappingContext(ControllerMappingContext, 0);
	}

	/*add actions*/
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInput)
	{
		EnhancedInput->BindAction(ToggleEscapeMenuAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerController::ToggleEscapeMenu);
		EnhancedInput->BindAction(ToggleServerInfoAction, ETriggerEvent::Triggered, this, &AFirstPersonPlayerController::ToggleServerInfo);

	}

	//InputComponent->BindAction("ToggleServerInfo", EInputEvent::IE_Pressed, this, &AFirstPersonPlayerController::ToggleServerInfo);

}

/*AcknowledgePossesion() - Event called whenever a client receives a new pawn*/
void AFirstPersonPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	SetInputMode(FInputModeGameOnly::FInputModeGameOnly());
	bShowMouseCursor = false;
	
	/*for local players only*/
	if (IsLocalController())
	{
		/*cast to our First Person Character*/
		if (AFirstPersonCharacter* FPC = Cast<AFirstPersonCharacter>(P))
		{
			/*Initiate HUD*/
			if (UFirstPersonViewComponent* FPV = FPC->GetFirstPersonView())
			{
				FPV->InitializePlayerHUD(this);
			}
			/*initialize Inventory Widget*/
			if (UInventoryManagerComponent* InvManager = FPC->GetInventoryManager())
			{
				InvManager->InitializeInventoryWidget(this);
			}
		}
	}
}




void AFirstPersonPlayerController::EnterSpectatorMode()
{
	if (GetNetMode() == NM_Client)
		return;


	if (GetPawn()) 
	{
		if (!GetPawn()->IsA<ASpectatorPawn>())
			UnPossess();			
	}
	AFirstPersonGame* FPGame = GetWorld()->GetAuthGameMode<AFirstPersonGame>();
	
	if (!FPGame)
		return;

	TSubclassOf<ASpectatorPawn> SpectatorClass = FPGame->GetSpectatorClass();
	FActorSpawnParameters SpawnParams;
	
	
	ASpectatorPawn* NewSpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass.Get(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (NewSpectatorPawn)
		Possess(NewSpectatorPawn);

}

void AFirstPersonPlayerController::InitializeEscapeMenuWidget(TSubclassOf<class UUserWidget> WidgetClass)
{
	if (IsLocalController() && WidgetClass)
		EscapeMenuWidget = CreateWidget<UUserWidget>(this, WidgetClass);
}

void AFirstPersonPlayerController::ToggleEscapeMenu()
{
	if (EscapeMenuWidget && EscapeMenuWidget->IsInViewport())
		CloseEscapeMenuWidget();
	else
		OpenEscapeMenuWidget();
}

void AFirstPersonPlayerController::OpenEscapeMenuWidget()
{
	if (EscapeMenuWidget && EscapeMenuWidget->IsInViewport() == false)
	{
		EscapeMenuWidget->AddToViewport();
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void AFirstPersonPlayerController::CloseEscapeMenuWidget()
{
	if (EscapeMenuWidget && EscapeMenuWidget->IsInViewport())
	{
		EscapeMenuWidget->RemoveFromParent();
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void AFirstPersonPlayerController::InitializeLoadoutWidget(TSubclassOf<class ULoadoutWidget> WidgetClass)
{
	if (IsLocalController() && WidgetClass)
		LoadoutWidget = CreateWidget<ULoadoutWidget>(this, WidgetClass);
}

void AFirstPersonPlayerController::OpenLoadoutWidget()
{
	if (LoadoutWidget && LoadoutWidget->IsInViewport() == false)
		LoadoutWidget->AddToViewport();
}

void AFirstPersonPlayerController::CloseLoadoutWidget()
{
	if (LoadoutWidget && LoadoutWidget->IsInViewport())
		LoadoutWidget->RemoveFromParent();
}

AInventoryLoadout* AFirstPersonPlayerController::GetLoadoutInfo()
{
	if (AFirstPersonPlayerState* PS = GetPlayerState<AFirstPersonPlayerState>())
	{
		return PS->Loadout;
	}
	else
		return nullptr;
}

void AFirstPersonPlayerController::SetLoadoutItem_PrimaryWeapon(TSubclassOf<AActor> Weapon)
{
	if (!Weapon)
		nullptr;

	if (AInventoryLoadout* Loadout = GetLoadoutInfo())
	{
		//Loadout->SetPrimaryWeapon(Weapon);
	}
}



void AFirstPersonPlayerController::InitializeServerInfoWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if(IsLocalController())
		ServerInfoWidget = CreateWidget<UUserWidget>(this, WidgetClass);
}

void AFirstPersonPlayerController::ToggleServerInfo()
{
	if (ServerInfoWidget && ServerInfoWidget->IsInViewport())
		CloseServerInfoWidget();
	else
		OpenServerInfoWidget();
}

void AFirstPersonPlayerController::OpenServerInfoWidget()
{
	if (ServerInfoWidget && ServerInfoWidget->IsInViewport() == false)
		ServerInfoWidget->AddToViewport();
}

void AFirstPersonPlayerController::CloseServerInfoWidget()
{
	if (ServerInfoWidget && ServerInfoWidget->IsInViewport())
		ServerInfoWidget->RemoveFromParent();
}

bool AFirstPersonPlayerController::ServerSelectNextMap_Validate(UMapAsset* RequestedMap)
{
	return true;
}

void AFirstPersonPlayerController::ServerSelectNextMap_Implementation(UMapAsset* RequestedMap)
{
	if (!RequestedMap)
		return;

	

	/*if the GameMode allows it - allow Clients to set NextMap*/
	if (AFirstPersonGame* GM = GetWorld()->GetAuthGameMode<AFirstPersonGame>())
	{
		if (GM->bAllowClientsToSetNextMap) //GameMode must allow this option
		{
			if (UServerSubsystem* SSS = GetGameInstance()->GetSubsystem<UServerSubsystem>())
			{
				SSS->SetNextMap(RequestedMap);
			}
		}		
	}	
}


void AFirstPersonPlayerController::ServerNotify_LevelInstanceLoadFinished_Implementation()
{
	AFirstPersonGame* AuthorityGameMode = GetWorld()->GetAuthGameMode<AFirstPersonGame>();
	if(AuthorityGameMode)
		AuthorityGameMode->OnPlayerLevelInstanceLoaded(this);
}

void AFirstPersonPlayerController::Server_UpdateLevelLoadPercent_Implementation(float NewPercent)
{
	AFirstPersonPlayerState* PS = GetPlayerState<AFirstPersonPlayerState>();

	if (PS)
		PS->LevelInstanceLoadPercent = NewPercent;
}