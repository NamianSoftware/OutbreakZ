// OutbreakZ: Mobile. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OutbreakZ_MobileEditorTarget : TargetRules
{
	public OutbreakZ_MobileEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "OutbreakZ_Mobile" } );
	}
}
