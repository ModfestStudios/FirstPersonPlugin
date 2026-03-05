// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/CinemaSubsystem.h"

/*cameras*/
#include "Camera/CameraComponent.h"


/*scene capture*/
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

/*materials*/
#include "Materials/MaterialInstanceDynamic.h"



void UCinemaSubsystem::SetViewTarget(AActor* NewViewTarget)
{
	if (!NewViewTarget)
		return;

	/*disable previous capture*/
	if (ActiveViewTarget != nullptr)
	{
		if (USceneCaptureComponent2D* PrevCaptureComponent = GetActiveSceneCaptureComponent())
		{
			PrevCaptureComponent->bCaptureEveryFrame = false;
			PrevCaptureComponent->bCaptureOnMovement = false;
		}
	}

	ActiveViewTarget = NewViewTarget;

	if (USceneCaptureComponent2D* CaptureComponent = GetActiveSceneCaptureComponent())
	{
		/*enable capture if not already*/
		CaptureComponent->bCaptureEveryFrame = true;
		CaptureComponent->bCaptureOnMovement = true;

		if (UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget.Get())
		{
			if (GetSceneCaptureMaterial())
				GetSceneCaptureMaterial()->SetTextureParameterValue(RenderTargetMaterialName, RenderTarget);
		}
		else if (FallbackRenderTarget)
		{
			CaptureComponent->TextureTarget = FallbackRenderTarget;
			
			if (GetSceneCaptureMaterial())
				GetSceneCaptureMaterial()->SetTextureParameterValue(RenderTargetMaterialName, FallbackRenderTarget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCinemaSubsystem::SetViewTarget() - NO TEXTURE TARGET SET ON SCENE CAPTURE COMPONENT && NO FALLBACK TEXTURE FOUND"));
		}
	}
	/*no render target was found - create one by default*/
	else
	{
		USceneCaptureComponent2D* NewCaptureComponent = NewObject<USceneCaptureComponent2D>(ActiveViewTarget,FName("Actor View Target"));
		NewCaptureComponent->TextureTarget = FallbackRenderTarget;
		NewCaptureComponent->bCaptureEveryFrame = true;
		NewCaptureComponent->bCaptureOnMovement = true;

		if (UCameraComponent* Camera = ActiveViewTarget->GetComponentByClass<UCameraComponent>())
		{
			NewCaptureComponent->AttachToComponent(Camera,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			NewCaptureComponent->bUseCameraHeightAsViewTarget = true;			
		}
		else
		{
			NewCaptureComponent->AttachToComponent(ActiveViewTarget->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}

		NewCaptureComponent->RegisterComponent();
		NewCaptureComponent->Activate();
	}
}

UMaterialInstanceDynamic* UCinemaSubsystem::InitiateSceneCaptureMaterial(UMaterialInterface* Material)
{
	if(!Material)
		return nullptr;

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this, FName("CinemaDynamicMaterial"));
	
	return DynamicMaterial;	
}

UMaterialInstanceDynamic* UCinemaSubsystem::GetSceneCaptureMaterial()
{
	return DynamicMaterial;
}

void UCinemaSubsystem::SetFallbackRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	FallbackRenderTarget = RenderTarget;
}

USceneCaptureComponent2D* UCinemaSubsystem::GetActiveSceneCaptureComponent()
{
	if (ActiveViewTarget)
	{
		return ActiveViewTarget->GetComponentByClass<USceneCaptureComponent2D>();
	}

	return nullptr;
}
