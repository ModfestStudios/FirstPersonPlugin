// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FirstPersonEditorModule : ModuleRules
{
	public FirstPersonEditorModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine",
				"AnimGraph",
				"BlueprintGraph",
				"FirstPersonModule"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"EditorStyle",
				"UnrealEd",
				"AssetTools",
				"KismetCompiler",
				"Kismet"  // Essential for Blueprint-related utilities
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
