using UnrealBuildTool;
using System.IO;
using System;

namespace UnrealBuildTool.Rules
{
    public class CNTKCore : ModuleRules
    {
        protected string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty")); }
        }

        public CNTKCore(ReadOnlyTargetRules Target) : base (Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PublicDependencyModuleNames.AddRange(new string[]{
				"Core",
				"CoreUObject",
                "Engine"
			});
            
            string CNTKPath = Path.Combine(ThirdPartyPath, "CNTK");

            string IncludeDir = Path.Combine(CNTKPath, "include");
            PublicIncludePaths.Add(IncludeDir);

            string LibDir = Path.Combine(CNTKPath, "lib");

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                // only support release
                LibDir = Path.Combine(LibDir, "Release");

                PublicLibraryPaths.Add(LibDir);
                PublicAdditionalLibraries.Add("Cntk.Core-2.5.1.lib");

                RuntimeDependencies.Add(Path.Combine(LibDir, "Cntk.Core-2.5.1.dll"));
                RuntimeDependencies.Add(Path.Combine(LibDir, "Cntk.Math-2.5.1.dll"));
            }
        }
    }
}