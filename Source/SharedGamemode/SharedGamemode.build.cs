// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SharedGamemode : ModuleRules
{
	public SharedGamemode(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        //PrivatePCHHeaderFile = "Public/SharedGamemode.h";

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
                "UMG",
				"InputCore",
				"DeveloperSettings",
				"GameFeatures",
				"ModularGameplay",
            });
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",				
			});
		
		
	
	}
}
