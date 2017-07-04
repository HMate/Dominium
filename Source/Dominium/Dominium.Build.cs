// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dominium : ModuleRules
{
	public Dominium(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        PublicDependencyModuleNames.AddRange(new string[] { "UnrealFastNoisePlugin", "RenderCore", "ShaderCore", "RHI", "RuntimeMeshComponent" });
    }
}
