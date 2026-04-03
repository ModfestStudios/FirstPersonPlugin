// Copyright (c) 2022 Pocket Sized Animations

using System.IO;
using UnrealBuildTool;

public class FirstPersonModule : ModuleRules
{
	public FirstPersonModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
                Path.Combine(ModuleDirectory, "Public")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] 
			{
				
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"EngineSettings",
				"InputCore",
                "OnlineSubsystem",
				"OnlineSubsystemUtils",
                "Niagara",
                "Networking",
                "NullDrv",
				"Settings",
                "RHI",
                "Sockets",
				"Voice"
            }
			);

		if(Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("PropertyEditor");

		};
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetRegistry",
				"Core",
				"CoreUObject",
				"Engine",
				"EngineSettings",
                "EnhancedInput",
                "Slate",
                "SlateCore",                
                "UMG",				
				"AIModule",
                "NullDrv",
                "OnlineSubsystem",
                "OnlineSubsystemUtils"
            }
			);
		
		if(Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			}
			);
	}
}
