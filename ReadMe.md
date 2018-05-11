## DeepLearningFramework in UE4

### Warning
> Read 'LICENSE.md'

Supprot DeepLearningFrameworks
* CNTK 2.5.1
* Tensorflow R1.7

### TODO
* Support Torch

### Sample

CNTK
```
auto func = UDeepFunctionLibrary::GetCNTKFunction(ECNTKDeviceType::GPU, "C:\\something.pretrained");
auto map = UDeepFunctionLibrary::GetEmptyEvaluateMap();
TArray<float> input = { 0., 0., 1., 0., 0., 1., 1., 1. };

UDeepFunctionLibrary::LoadEmptyEvaluateMap(map, ECNTKDeviceType::CPU, func, "", input);

auto output = UDeepFunctionLibrary::RunCNTK(func, map);
```

Tensorflow
```
FString GraphDef = "C:\\something.pb";

FTensorflowSession session = UDeepFunctionLibrary::CreateSession(GraphDef);
if(!session.Handle.IsValid())
{
	return false;
}
 
UDeepFunctionLibrary::PushInput(session, FString(TEXT("a")), ETensorflowDataType::DT_FLOAT, { 1 });
UDeepFunctionLibrary::PushInput(session, FString(TEXT("b")), ETensorflowDataType::DT_FLOAT, { 1 });
UDeepFunctionLibrary::LoadInput(session, FString(TEXT("a")));

session.FlatFloat.Reset();
session.FlatFloat.Add(3.0f);
if (!UDeepFunctionLibrary::Write(session, ETensorflowDataType::DT_FLOAT))
{
	return false;
}

UDeepFunctionLibrary::LoadInput(session, FString(TEXT("b")));
session.FlatFloat.Reset();
session.FlatFloat.Add(2.0f);
if (!UDeepFunctionLibrary::Write(session, ETensorflowDataType::DT_FLOAT))
{
	return false;
}

UDeepFunctionLibrary::RunTS(session, { FString(TEXT("c")) });
if (UDeepFunctionLibrary::GetStatus(session) != "OK")
{
	return false;
}

UDeepFunctionLibrary::LoadOutput(session, FString(TEXT("c")));
if (!UDeepFunctionLibrary::Read(session, ETensorflowDataType::DT_FLOAT))
{
	return false;
}

auto output = session.FlatFloat;

return true;
```