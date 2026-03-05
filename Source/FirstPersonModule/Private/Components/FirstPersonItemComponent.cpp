// Copyright (c) 2022 Pocket Sized Animations


#include "Components/FirstPersonItemComponent.h"
#include "FirstPersonModule/Public/Components/FirstPersonViewComponent.h"

/*animations*/
#include "Animation/AnimInstance.h"

/*components*/
#include "Components/SkeletalMeshComponent.h"


/*framework*/
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"



// Sets default values for this component's properties
UFirstPersonItemComponent::UFirstPersonItemComponent()
{

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}


void UFirstPersonItemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitializeFirstPersonMesh();
}

// Called when the game starts
void UFirstPersonItemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UFirstPersonItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFirstPersonItemComponent::InitializeFirstPersonMesh()
{
	if (SkeletalMeshComp != nullptr || FirstPersonMesh == nullptr)
		return;

	/*create first person mesh*/
	SkeletalMeshComp = NewObject<USkeletalMeshComponent>(GetOwner(), MeshName, RF_Transient);
	if (SkeletalMeshComp)
	{
		/*initialize*/
		SkeletalMeshComp->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SkeletalMeshComp->RegisterComponent();
		SkeletalMeshComp->Activate();

		/*setup placement*/
		//SkeletalMeshComp->SetRelativeLocation(ArmsOffset);

		/*initialize visuals*/
		SkeletalMeshComp->SetSkeletalMesh(FirstPersonMesh);
		SkeletalMeshComp->SetAnimInstanceClass(AnimationBlueprint);
		SkeletalMeshComp->ResetAnimInstanceDynamics(ETeleportType::ResetPhysics);

		/*set it so only the owning player sees this version*/
		SkeletalMeshComp->SetOnlyOwnerSee(true);
	}
}

void UFirstPersonItemComponent::AttachFirstPersonMesh(AActor* OwningActor)
{
	/*safety check*/
	if (!SkeletalMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s:FirstPersonItemComponent::AttachFirstPersonMesh() - NO FIRST PERSON MESH ASSIGNED"),*GetNameSafe(GetOwner()));
		return;
	}
		

	/*if nothing is passed in - try to dynamically find the owning actor by getting the pawn*/
	if (OwningActor == nullptr )
		OwningActor = GetOwningPawn();

	if (OwningActor)
	{
		if (UFirstPersonViewComponent* FPV = Cast<UFirstPersonViewComponent>(OwningActor->GetComponentByClass(UFirstPersonViewComponent::StaticClass())))
		{
			if (FPV->GetArmsMeshComponent())
			{
				SkeletalMeshComp->AttachToComponent(FPV->GetArmsMeshComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GetAttachmentSocketName());
			}
		}
	}
}

FName UFirstPersonItemComponent::GetAttachmentSocketName()
{
	//@TODO : update this to be more dynamic
	return FName("RightHandSocket");
}

APawn* UFirstPersonItemComponent::GetOwningPawn()
{
	APawn* _pawn = nullptr;
	AActor* _owner = GetOwner();

	while (_pawn == nullptr)
	{
		if (_owner == nullptr)
			return nullptr;

		_pawn = Cast<APawn>(_owner);
		if (_pawn)
			return _pawn;

		else
			_owner = _owner->GetOwner();
	}

	return nullptr;
}

