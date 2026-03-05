// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team7_CH3_Project : ModuleRules
{
	public Team7_CH3_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",
            "Niagara",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "MediaAssets"
        });
    }
}
