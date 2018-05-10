#include "CNTKCorePlugin.h"

class CNTKCORE_API FCNTKCore : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TArray<void*> DLLHandleList;
};

IMPLEMENT_MODULE(FCNTKCore, CNTKCore)

void FCNTKCore::StartupModule()
{
	TArray<FString> DependencyDLLNames = {
		"libiomp5md.dll",
		"mkl_cntk_p.dll",
		// GPU depedency dll
		"cublas64_90.dll",
		"cudart64_90.dll",
		"cudnn64_7.dll",
		"curand64_90.dll",
		"cusparse64_90.dll",
		"nvml.dll"
	};
	const FString CNTKPath = FPaths::ProjectDir() / TEXT("Plugins/Runtime/CNTK/ThirdParty/lib/");
	FPlatformProcess::PushDllDirectory(*CNTKPath);
	for (auto path : DependencyDLLNames)
	{
		DLLHandleList.Add(FPlatformProcess::GetDllHandle(*(CNTKPath + path)));
	}
	FPlatformProcess::PopDllDirectory(*CNTKPath);

// #if UE_BUILD_DEVELOPMENT_WITH_DEBUGGAME || UE_BUILD_DEBUG
// 	TArray<FString> DebugDLLNames = {
// 		"Cntk.Core-2.5.1d.dll",
// 		"Cntk.Math-2.5.1d.dll"
// 	};
// 	const FString DebugCNTKPath = CNTKPath + "Debug/";
// 	FPlatformProcess::PushDllDirectory(*DebugCNTKPath);
// 	for (auto path : DebugDLLNames)
// 	{
// 		DLLHandleList.Add(FPlatformProcess::GetDllHandle(*(DebugCNTKPath + path)));
// 	}
// 	FPlatformProcess::PopDllDirectory(*DebugCNTKPath);
// #else
	TArray<FString> ReleaseDLLNames = {
		"Cntk.Core-2.5.1.dll",
		"Cntk.Math-2.5.1.dll"
	};
	const FString ReleaseCNTKPath = CNTKPath + "Release/";
	FPlatformProcess::PushDllDirectory(*ReleaseCNTKPath);
	for (auto path : ReleaseDLLNames)
	{
		DLLHandleList.Add(FPlatformProcess::GetDllHandle(*(ReleaseCNTKPath + path)));
	}
	FPlatformProcess::PopDllDirectory(*ReleaseCNTKPath);
// #endif
}


void FCNTKCore::ShutdownModule()
{
	for (auto dllHandle : DLLHandleList)
	{
		FPlatformProcess::FreeDllHandle(dllHandle);
		dllHandle = nullptr;
	}
}
