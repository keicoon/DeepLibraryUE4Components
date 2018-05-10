#include "TensorflowBridgePlugin.h"

class TENSORFLOWBRIDGE_API FTensorflowBridgePlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void* DLLHandle;
};

IMPLEMENT_MODULE(FTensorflowBridgePlugin, Tensorflow)

void FTensorflowBridgePlugin::StartupModule()
{
	const FString TensorflowPath = FPaths::ProjectPluginsDir() / TEXT("DeepLibrary/ThirdParty/Tensorflow/lib/");
	FPlatformProcess::PushDllDirectory(*TensorflowPath);
	DLLHandle = FPlatformProcess::GetDllHandle(*(TensorflowPath + "dl_adaptor.dll"));
	FPlatformProcess::PopDllDirectory(*TensorflowPath);
}

void FTensorflowBridgePlugin::ShutdownModule()
{
	if (DLLHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(DLLHandle);
		DLLHandle = nullptr;
	}
}