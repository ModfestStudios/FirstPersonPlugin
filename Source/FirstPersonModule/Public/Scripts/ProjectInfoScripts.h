// Copyrighted Modfest Studios && Micah A. Parker 2025

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectInfoScripts.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API UProjectInfoScripts : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Project")
		static FString GetProjectName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Project")
		static FString GetProjectDebugName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Project")
		static FString GetProjectVersion();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Project")
		static FString GetProjectDescription();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Project")
		static FString GetProjectID();
	
};
