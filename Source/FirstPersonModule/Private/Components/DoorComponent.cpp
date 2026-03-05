// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DoorComponent.h"
#include "Components/BillboardComponent.h"

#include "Net/UnrealNetwork.h"


void UDoorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UDoorComponent, bOpened);
    
}

UDoorComponent::UDoorComponent(const FObjectInitializer& ObjectInitializer)
{
    SetComponentTickEnabled(true);
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = true;
}

#if WITH_EDITOR
void UDoorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UDoorComponent, bOpened))
    {
        UpdateDoorPosition();
    }

}
#endif

void UDoorComponent::OnRegister()
{
    Super::OnRegister();

    UpdateDoorPosition();    
}

void UDoorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bOpened)
        DoorState = EDoorState::Opened;
    else
        DoorState = EDoorState::Closed;

    //RegisterComponent();
    //SetComponentTickEnabled(true);
    //RegisterComponentTickFunctions(true);

    if (!OpenAnimation.AnimationCurve)
        OpenAnimation.AnimationCurve = GetDefaultCurve();
    if (!CloseAnimation.AnimationCurve)
        CloseAnimation.AnimationCurve = GetDefaultCurve();

}

void UDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
   

    /*animate door*/
    if (DoorTimeline.IsPlaying())
    {        
        DoorTimeline.TickTimeline(DeltaTime);
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UDoorComponent::OpenDoor()
{
    if (CanDoorOpen())
    {
        bOpened = true;
        PlayDoorAnimation(OpenAnimation);
        DoorState = EDoorState::Opening;
    }
		
}

void UDoorComponent::CloseDoor()
{
	if (CanDoorClose())
    {
        bOpened = false;
        PlayDoorAnimation(CloseAnimation);
        DoorState = EDoorState::Closing;
    }	
}

bool UDoorComponent::CanDoorOpen()
{
	/*doors already opened*/
	if (DoorState == EDoorState::Opened || DoorState == EDoorState::Opening)
		return false;

	/*doors locked*/
	if (bLocked == true)
		return false;

	return true;
}

bool UDoorComponent::CanDoorClose()
{
	/*doors already closed or closing*/
	if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
		return false;

    return true;	
}

//void UDoorComponent::UpdateDoorPosition()
//{
//    if (GetDoorPivot())
//    {
//        FRotator NewRotation = GetDoorPivot()->GetComponentRotation();
//
//        // Set to open or closed angle
//        NewRotation.Yaw = bOpened ? OpenAnimation.TargetDistance : 0.0f;
//
//        GetDoorPivot()->SetRelativeRotation(NewRotation);
//    }
//}

/*UpdateDoorPosition - Used to jump the door to this position when working in an editor or replicating a state to a client that doesn't need to see the animation (cause it's too late)*/
void UDoorComponent::UpdateDoorPosition()
{
    if (GetDoorPivot())
    {
        float Direction = bReverseDirection ? -1.0f : 1.0f;

        switch (DoorType)
        {
        case EDoorType::Swing:
        {
            // Rotate around the Yaw axis for swinging doors
            FRotator NewRotation = GetDoorPivot()->GetRelativeRotation();
            NewRotation.Yaw = bOpened ? (OpenAnimation.TargetDistance * Direction) : 0.0f;
            GetDoorPivot()->SetRelativeRotation(NewRotation);
            break;
        }
        case EDoorType::SlideHorizontal:
        {
            // Translate along the X axis for horizontal sliding doors
            FVector NewLocation = GetDoorPivot()->GetRelativeLocation();
            NewLocation.Y = bOpened ? (OpenAnimation.TargetDistance * Direction) : 0.0f;
            GetDoorPivot()->SetRelativeLocation(NewLocation);
            break;
        }
        case EDoorType::SlideVertical:
        {
            // Translate along the Z axis for vertical sliding doors
            FVector NewLocation = GetDoorPivot()->GetRelativeLocation();
            NewLocation.Z = bOpened ? (OpenAnimation.TargetDistance * Direction)  : 0.0f;
            GetDoorPivot()->SetRelativeLocation(NewLocation);
            break;
        }
        case EDoorType::DrawBridge:
        {
            // Rotate around the Pitch axis for drawbridge doors
            FRotator NewRotation = GetDoorPivot()->GetRelativeRotation();
            NewRotation.Pitch = bOpened ? (OpenAnimation.TargetDistance * Direction) : 0.0f;
            GetDoorPivot()->SetRelativeRotation(NewRotation);
            break;
        }
        default:
            // Handle unknown or custom door types
            UE_LOG(LogTemp, Warning, TEXT("Unknown DoorType in UpdateDoorPosition!"));
            break;
        }
    }
}

void UDoorComponent::OnRep_OpenedStateChanged()
{
    if (bOpened)
        OpenDoor();
    else
        CloseDoor();
}



bool UDoorComponent::IsAnimationInProgress()
{
    return DoorTimeline.IsPlaying();
}


void UDoorComponent::PlayDoorAnimation(const FDoorAnimation& DoorAnimation, bool bReverse)
{
    UE_LOG(LogTemp, Log, TEXT("PlayDoorAnimation()"));

    if (!DoorAnimation.AnimationCurve) return;

    CurrentAnimation = DoorAnimation;

    FOnTimelineFloat ProgressFunction;
    ProgressFunction.BindUFunction(this, FName("AnimateDoor"));

    FOnTimelineEvent TimelineFinished;
    TimelineFinished.BindUFunction(this, FName("OnAnimationFinished"));

    DoorTimeline = FTimeline();
    DoorTimeline.AddInterpFloat(DoorAnimation.AnimationCurve, ProgressFunction);
    DoorTimeline.SetTimelineFinishedFunc(TimelineFinished);
    DoorTimeline.SetLooping(false);
    DoorTimeline.SetTimelineLength(DoorAnimation.AnimationDuration);
    DoorTimeline.SetPlayRate(1.0f / DoorAnimation.AnimationDuration);

    if (bReverse)
    {
        DoorTimeline.ReverseFromEnd();
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Start Animation"));
        DoorTimeline.PlayFromStart();
    }
}

void UDoorComponent::AnimateDoor(float Value)
{
    UE_LOG(LogTemp, Log, TEXT("%s::AnimateDoor(%f)"), *GetName(), Value);
    if (GetDoorPivot())
    {
        FTransform CurrentTransform = GetDoorPivot()->GetRelativeTransform();
        FVector NewLocation = CurrentTransform.GetLocation();
        FRotator NewRotation = CurrentTransform.GetRotation().Rotator();
        float Direction = bReverseDirection ? -1.0f : 1.0f;

        switch (DoorType)
        {
        case EDoorType::Swing:
            NewRotation.Yaw = FMath::Lerp(0.0f, (CurrentAnimation.TargetDistance * Direction), Value);
            UE_LOG(LogTemp, Log, TEXT("\t Swing Door - New Yaw = %f"), NewRotation.Yaw);
            break;

        case EDoorType::SlideHorizontal:
            NewLocation.Y = FMath::Lerp(0.0f, (CurrentAnimation.TargetDistance * Direction), Value);
            UE_LOG(LogTemp, Log, TEXT("\t Slide Horizontal - New Y = %f"), NewLocation.Y);
            break;
            
        case EDoorType::SlideVertical:
            NewLocation.Z = FMath::Lerp(0.0f, (CurrentAnimation.TargetDistance * Direction), Value);
            UE_LOG(LogTemp, Log, TEXT("\t Slide Vertical - New Z = %f"), NewLocation.Z);
            break;

        case EDoorType::DrawBridge:
            NewRotation.Pitch = FMath::Lerp(0.0f, (CurrentAnimation.TargetDistance * Direction), Value);
            UE_LOG(LogTemp, Log, TEXT("\t DrawBridge Door - New Pitch = %f"), NewRotation.Pitch);
            break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("\t Unknown DoorType!"));
            return;
        }

        // Apply the calculated transformations
        GetDoorPivot()->SetRelativeLocationAndRotation(NewLocation, NewRotation.Quaternion());
    }
}

void UDoorComponent::OnAnimationFinished()
{    
    if (DoorState == EDoorState::Opening)
        DoorState = EDoorState::Opened;
    else if (DoorState == EDoorState::Closing)
        DoorState = EDoorState::Closed;
}

UCurveFloat* UDoorComponent::GetDefaultCurve()
{
    UCurveFloat* FallbackCurve = NewObject<UCurveFloat>(this, UCurveFloat::StaticClass());
    FRichCurve* CurveData = &FallbackCurve->FloatCurve;

    CurveData->AddKey(0.0f, 0.0f);
    CurveData->AddKey(1.0f, 1.0f);
    
    return FallbackCurve;
}

USceneComponent* UDoorComponent::GetDoorPivot()
{
    return GetAttachParent();
}

