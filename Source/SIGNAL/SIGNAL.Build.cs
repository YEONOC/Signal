// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SIGNAL : ModuleRules
{
	public SIGNAL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 루트 폴더 인식
        PublicIncludePaths.AddRange(new string[] { "SIGNAL" });


        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SIGNAL",
			"SIGNAL/Variant_Platforming",
			"SIGNAL/Variant_Platforming/Animation",
			"SIGNAL/Variant_Combat",
			"SIGNAL/Variant_Combat/AI",
			"SIGNAL/Variant_Combat/Animation",
			"SIGNAL/Variant_Combat/Gameplay",
			"SIGNAL/Variant_Combat/Interfaces",
			"SIGNAL/Variant_Combat/UI",
			"SIGNAL/Variant_SideScrolling",
			"SIGNAL/Variant_SideScrolling/AI",
			"SIGNAL/Variant_SideScrolling/Gameplay",
			"SIGNAL/Variant_SideScrolling/Interfaces",
			"SIGNAL/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
