// OutbreakZ: Mobile. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OutbreakZ_MobileTarget : TargetRules
{
	public OutbreakZ_MobileTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "OutbreakZ_Mobile" } );
	}
}
