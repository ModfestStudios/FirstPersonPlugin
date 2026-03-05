// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactives/Actors/CounterClock.h"
#include "Components/CounterComponent.h"
#include "Components/TextRenderComponent.h"
// Sets default values
ACounterClock::ACounterClock(const FObjectInitializer& ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComp"));
	if (Mesh)
	{
		SetRootComponent(Mesh);
	}
	TextRender = ObjectInitializer.CreateDefaultSubobject<UTextRenderComponent>(this, TEXT("TextRenderComp"));
	if (TextRender)
	{
		TextRender->SetupAttachment(Mesh);
		TextRender->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
		TextRender->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	}
	Counter = ObjectInitializer.CreateDefaultSubobject<UCounterComponent>(this, TEXT("CounterComp")); 	

	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACounterClock::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACounterClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*update time*/
	if (Counter && Counter->IsCountdownActive())
	{
		FText TimeReaminingText = Counter->GetTimeRemainingAsText();
		TextRender->SetText(TimeReaminingText);
	}
}

void ACounterClock::StartCounter()
{
	if (Counter)
		Counter->StartCounter();
}

void ACounterClock::StopCounter()
{
	if (Counter)
		Counter->StopCounter();
}

void ACounterClock::SetCountdownDuration(float NewDuration)
{
	if (Counter)
		Counter->SetCountdownDuration(NewDuration);
}

