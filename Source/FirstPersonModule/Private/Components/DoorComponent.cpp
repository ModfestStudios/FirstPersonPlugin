// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DoorComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"

#include "Interactives/Actions/OpenAction.h"
#include "Interactives/Actions/CloseAction.h"
#include "Interactives/Actions/LockAction.h"
#include "Interactives/Actions/UnlockAction.h"

#include "GameModes/FirstPersonGameState.h"

#include "Net/UnrealNetwork.h"


void UDoorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UDoorComponent, DoorReplicatedState)   
}

UDoorComponent::UDoorComponent(const FObjectInitializer& ObjectInitializer)
{
    /*default actions*/
    ActionConfigs.Add(FInteractiveActionConfig(UOpenAction::StaticClass()));
    ActionConfigs.Add(FInteractiveActionConfig(UCloseAction::StaticClass()));
    ActionConfigs.Add(FInteractiveActionConfig(ULockAction::StaticClass()));
    ActionConfigs.Add(FInteractiveActionConfig(UUnlockAction::StaticClass()));

    /*setup default animations*/
    CloseAnimation.TargetPosition = 0.0f;

    OpenAnimation.TargetPosition = 100.0f;

    SetComponentTickEnabled(true);
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = true;
}

void UDoorComponent::InitializeComponent()
{
    Super::InitializeComponent();

}

#if WITH_EDITOR
void UDoorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    //if (PropertyName == GET_MEMBER_NAME_CHECKED(UDoorComponent, DoorMesh))
    //{
    //    UpdateDoorMesh();
    //}

    //if (PropertyName == GET_MEMBER_NAME_CHECKED(UDoorComponent, bOpened))
    //{
    //    UpdateDoorPosition();
    //}

}
#endif

void UDoorComponent::OnRegister()
{
    Super::OnRegister();

    //UpdateDoorMesh();
    //UpdateDoorPosition();    
}

void UDoorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (USceneComponent* DoorPivot = GetDoorPivot())    
        InitialDoorTransform = DoorPivot->GetRelativeTransform();
    

    if (!OpenAnimation.bAnimateUsingCurve || !OpenAnimation.AnimationCurve)
        OpenAnimation.AnimationCurve = GetDefaultLinearCurve();

    if (!CloseAnimation.bAnimateUsingCurve || !CloseAnimation.AnimationCurve)
        CloseAnimation.AnimationCurve = GetDefaultLinearCurve();
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

//======================================
//=============INTERACTIONS=============
//======================================


void UDoorComponent::OpenDoor()
{
    if (CanDoorOpen())
    {        
        SetDoorState(EDoorState::Opening);
    }
    else
    {

    }
}

void UDoorComponent::CloseDoor()
{
    if (CanDoorClose())
    {        
        SetDoorState(EDoorState::Closing);
    }
}

void UDoorComponent::StopDoor()
{
    
}


//===============================
//=============STATE=============
//===============================

EDoorState UDoorComponent::GetCurrentDoorState() const
{
    return CurrentState;
}

EDoorState UDoorComponent::GetPreviousDoorState() const
{
    return PreviousState;
}

bool UDoorComponent::IsDoorOpen() const
{
   return CurrentState != EDoorState::Closed;
}

bool UDoorComponent::IsDoorOpening() const
{
    return CurrentState == EDoorState::Opening;
}

bool UDoorComponent::CanDoorOpen() const
{    
    if(IsDoorClosed())
        return true;

    return false;
}

bool UDoorComponent::IsDoorClosed() const
{
    return CurrentState == EDoorState::Closed;
}

bool UDoorComponent::IsDoorClosing() const
{
    return CurrentState == EDoorState::Closing;
}

bool UDoorComponent::CanDoorClose() const
{
    return IsDoorOpen();
}

bool UDoorComponent::IsDoorStopped() const
{
    return CurrentState == EDoorState::Stopped;
}

bool UDoorComponent::CanDoorStop() const
{
    return CurrentState == EDoorState::Opening || CurrentState == EDoorState::Closing;
}

bool UDoorComponent::IsDoorLocked() const
{
    return bLocked;
}

bool UDoorComponent::CanDoorBeLocked() const
{
    return false;
}

bool UDoorComponent::DoesLockRequireKey() const
{
    return false;
}


void UDoorComponent::SetDoorState(EDoorState NewState)
{
    if (NewState == CurrentState)
        return;

    PreviousState = CurrentState;
    CurrentState = NewState;

    /*server prepares authoritative replicated state*/
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;

        DoorReplicatedState.NewState = NewState;
        DoorReplicatedState.PreviousState = PreviousState;
        DoorReplicatedState.ServerStartTime = GameState ? GameState->GetServerWorldTimeSeconds() : 0.0f;
        DoorReplicatedState.CurrentPosition = DoorPosition;

        /*we want to force this since doors are gameplay important - if someone opens a door and we get a small delay before it replicates naturally the receiving client may already be dead from the threat*/
        GetOwner()->ForceNetUpdate();
    }

    /*server and client both perform their local state behavior*/
    OnDoorStateChanged(CurrentState, PreviousState);
}

void UDoorComponent::OnDoorStateChanged(EDoorState NewState, EDoorState PrevState)
{
    switch (NewState)
    {
        case EDoorState::Opening :
            {                
                PlayDoorAnimation(OpenAnimation);
            }
        break;
        case EDoorState::Closing :
            {               
               PlayDoorAnimation(CloseAnimation);
            }
        break;
        case EDoorState::Opened :
            {

            }
        break;
        case EDoorState::Closed :
            {

            }        
        case EDoorState::Stopped :
        default:
            {
                
            }
        break;
    }
}

void UDoorComponent::OnRep_DoorReplicatedState()
{
    PreviousState = DoorReplicatedState.PreviousState;
    CurrentState = DoorReplicatedState.NewState;

    /*correct our local position to the server's position at state change*/
    DoorPosition = DoorReplicatedState.CurrentPosition;
    UpdateDoorTransform();

    const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
    const float CurrentServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : DoorReplicatedState.ServerStartTime;
    const float TimeSinceStateChange = CurrentServerTime - DoorReplicatedState.ServerStartTime; 

    if (TimeSinceStateChange <= MaxAnimationReplicationDelay)
    {
        OnDoorStateChanged(CurrentState, PreviousState);
    }
    else
    {
        SnapDoorToState(CurrentState);
    }
}


bool UDoorComponent::IsAnimationInProgress()
{
    return DoorTimeline.IsPlaying();
}


void UDoorComponent::PlayDoorAnimation(const FDoorAnimation& DoorAnimation)
{
    if (!DoorAnimation.AnimationCurve)
        return;

    /*stop/reset any current animation*/
    DoorTimeline.Stop();
    DoorTimeline = FTimeline();

    /*copy configuration into our runtime animation*/
    CurrentAnimation = DoorAnimation;

    /*capture where the door currently is*/
    AnimationStartPosition = DoorPosition;

    /*already at the target*/
    if (FMath::IsNearlyEqual(AnimationStartPosition, CurrentAnimation.TargetPosition))
    {
        DoorPosition = CurrentAnimation.TargetPosition;
        UpdateDoorTransform();
        OnAnimationFinished();
        return;
    }

    FOnTimelineFloat ProgressFunction;
    ProgressFunction.BindUFunction(this, FName("AnimateDoor"));

    FOnTimelineEvent FinishedFunction;
    FinishedFunction.BindUFunction(this, FName("OnAnimationFinished"));

    DoorTimeline.AddInterpFloat(CurrentAnimation.AnimationCurve, ProgressFunction);
    DoorTimeline.SetTimelineFinishedFunc(FinishedFunction);
    DoorTimeline.SetLooping(false);

    DoorTimeline.SetTimelineLength(1.0f);
    DoorTimeline.SetPlayRate(1.0f / FMath::Max(CurrentAnimation.AnimationDuration, KINDA_SMALL_NUMBER));

    DoorTimeline.PlayFromStart();
}

void UDoorComponent::AnimateDoor(float Value)
{
    DoorPosition = FMath::Lerp(
        AnimationStartPosition,
        CurrentAnimation.TargetPosition,
        Value
    );

    UpdateDoorTransform();
}

void UDoorComponent::OnAnimationFinished()
{
    DoorPosition = CurrentAnimation.TargetPosition;
    UpdateDoorTransform();

    if (CurrentState == EDoorState::Opening)
    {
        SetDoorState(EDoorState::Opened);
    }
    else if (CurrentState == EDoorState::Closing)
    {
        SetDoorState(EDoorState::Closed);
    }
}

void UDoorComponent::UpdateDoorTransform()
{
    USceneComponent* DoorPivot = GetDoorPivot();

    if (!DoorPivot)
        return;

    FVector NewLocation = InitialDoorTransform.GetLocation();
    FRotator NewRotation = InitialDoorTransform.GetRotation().Rotator();

    switch (DoorType)
    {
    case EDoorType::Swing:
        {
            NewRotation.Yaw += DoorPosition;
            break;
        }

    case EDoorType::SlideHorizontal:
        {
            NewLocation.Y += DoorPosition;
            break;
        }

    case EDoorType::SlideVertical:
        {
            NewLocation.Z += DoorPosition;
            break;
        }

    case EDoorType::DrawBridge:
        {
            NewRotation.Pitch += DoorPosition;
            break;
        }

    default:
        return;
    }

    DoorPivot->SetRelativeLocationAndRotation(NewLocation, NewRotation);
}

void UDoorComponent::SnapDoorToState(EDoorState State)
{
    DoorTimeline.Stop();

    switch (State)
    {
    case EDoorState::Opening:
    case EDoorState::Opened:
        DoorPosition = OpenAnimation.TargetPosition;
        break;

    case EDoorState::Closing:
    case EDoorState::Closed:
        DoorPosition = CloseAnimation.TargetPosition;
        break;

    case EDoorState::Stopped:
        DoorPosition = DoorReplicatedState.CurrentPosition;
        break;

    default:
        return;
    }

    UpdateDoorTransform();
}

UCurveFloat* UDoorComponent::GetDefaultLinearCurve()
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

