// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Traps/TrapBase.h"
#include "SawBladeTrap.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ASawBladeTrap : public ATrap
{
	GENERATED_BODY()
public:

	/*the name of the bone the collision should attach to (it should be the blade...probably)*/
	UPROPERTY(EditAnywhere, Category = "Trap")
		FName BladeAttachmentSocketName = "b_Blade";

	UPROPERTY(EditAnywhere, Category = "Collision")
		class UBoxComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* Mesh;	

	//=======================================================================================================================================
	//===============================================================FUNCTIONS===============================================================
	//=======================================================================================================================================


	ASawBladeTrap(const FObjectInitializer& ObjectInitializer);

public:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnBladeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap", meta=(DisplayName="On Blade Overlap"))
		void BP_OnBladeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnBladeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap", meta = (DisplayName = "On Blade End Overlap"))
		void BP_OnBladeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	





	
};
