// OutbreakZ: Mobile. All rights reserved.

using UnrealBuildTool;

public class OutbreakZ_Mobile : ModuleRules
{
	public OutbreakZ_Mobile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "ChaosVehicles", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(
			new string[]
			{
				"OutbreakZ_Mobile/Public/Components",
				"OutbreakZ_Mobile/Public/Player",
				"OutbreakZ_Mobile/Public/Vehicle",
				"OutbreakZ_Mobile/Public/UI",
				"OutbreakZ_Mobile/Public/World"
			}
		);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}