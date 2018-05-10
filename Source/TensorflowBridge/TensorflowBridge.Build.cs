using UnrealBuildTool;
using System.IO;
using System;

namespace UnrealBuildTool.Rules
{
	public class TensorflowBridge : ModuleRules
	{
        protected string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty")); }
        }

        public TensorflowBridge(ReadOnlyTargetRules Target) : base (Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PublicDependencyModuleNames.AddRange(new string[]{
					"Core",
					"CoreUObject",
                    "Engine",
			});

            string TensorflowPath = Path.Combine(ThirdPartyPath, "Tensorflow");

            string IncludeDir = Path.Combine(TensorflowPath, "include");
            PublicIncludePaths.Add(IncludeDir);

            string LibDir = Path.Combine(TensorflowPath, "lib");

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicLibraryPaths.Add(LibDir);
                PublicAdditionalLibraries.Add("dl_adaptor_g.lib");
                PublicDelayLoadDLLs.Add("dl_adaptor_g.dll");

                RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibDir, "dl_adaptor_g.dll")));
            }
        }
	}
}
