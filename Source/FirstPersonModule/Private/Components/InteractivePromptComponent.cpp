// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractivePromptComponent.h"
#include "UI/Components/Infos/InteractivePromptInfo.h"


/*utilities*/
#include "UObject/ConstructorHelpers.h"

#if WITH_EDITOR
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#endif


UInteractivePromptComponent::UInteractivePromptComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/FirstPersonModule/UI/Components/Infos/INFO_InteractivePrompt"));
	if (WidgetClassFinder.Succeeded())
		SetWidgetClass(WidgetClassFinder.Class);
	SetDrawSize(FVector2D(512, 96));
	SetPivot(FVector2D(0, 0.5));
	SetRelativeScale3D(FVector(1.0, 0.09375, 0.09375));

	bCastDynamicShadow = false;	
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

#if WITH_EDITOR
void UInteractivePromptComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{	
	SetWorldSpecificComponentVisiblity();
	Super::PostEditChangeProperty(PropertyChangedEvent);

}

void UInteractivePromptComponent::PostEditComponentMove(bool bFinished)
{
	SetWorldSpecificComponentVisiblity();

	Super::PostEditComponentMove(bFinished);

}
#endif

void UInteractivePromptComponent::InitializeComponent()
{
	SetWorldSpecificComponentVisiblity();
	Super::InitializeComponent();
}

void UInteractivePromptComponent::OnRegister()
{

	/*hides the widget when being placed in the world to avoid clutter*/
	SetWorldSpecificComponentVisiblity();

	Super::OnRegister();
}



void UInteractivePromptComponent::BeginPlay()
{
	SetVisibility(false);
	Super::BeginPlay();
}

void UInteractivePromptComponent::SetWorldSpecificComponentVisiblity()
{
	#if WITH_EDITOR
	if (GIsEditor && (GetWorld() && !GetWorld()->IsGameWorld()))
	{
		const bool bInBlueprintEditor = !GCurrentLevelEditingViewportClient || !GCurrentLevelEditingViewportClient->IsVisible();
		SetVisibility(bInBlueprintEditor);  // Visible in Blueprint Editor, Hidden in Level View
	}
	
	else
		SetVisibility(false); //default
#endif
}

void UInteractivePromptComponent::SetPromptText(FText NewText)
{
	if (UInteractivePromptInfo* PromptWidget = Cast<UInteractivePromptInfo>(GetWidget()))
	{
		PromptWidget->SetActionLabel(NewText);
	}
}
