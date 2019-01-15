// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class ExilesOfEternityServerTarget : TargetRules
{
       public ExilesOfEternityServerTarget(TargetInfo Target) : base(Target)
       {
        Type = TargetType.Server;
        ExtraModuleNames.Add("ExilesOfEternity");
       }
}
