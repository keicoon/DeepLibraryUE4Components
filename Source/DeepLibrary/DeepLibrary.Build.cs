using UnrealBuildTool;
using System.IO;
using System;

namespace UnrealBuildTool.Rules
{
	public class DeepLibrary : ModuleRules
	{
        protected string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty")); }
        }
        
        public DeepLibrary(ReadOnlyTargetRules Target) : base (Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PublicIncludePaths.AddRange(new string[]{
                "DeepLibrary/Classes"
            });

            PublicDependencyModuleNames.AddRange(new string[]{
				"Core",
				"CoreUObject",
                "Engine",
                "CNTKCore",
                "TensorflowBridge"
			});
        }
	}
}
