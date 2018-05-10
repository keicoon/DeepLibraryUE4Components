#include "DeepLibraryPlugin.h"

#include "CNTKLibrary.h"
#include "dl-tensorflow.h"

#include "DeepFunctionLibrary.h"

class FDeepLibrary : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FDeepLibrary::StartupModule()
{
}

void FDeepLibrary::ShutdownModule()
{
}

namespace CNTKHelper
{
	CNTK::DeviceDescriptor GetDeviceDescriptor(ECNTKDeviceType DeviceType)
	{
		if (DeviceType == ECNTKDeviceType::CPU)
		{
			return CNTK::DeviceDescriptor::CPUDevice();
		}
		else
		{
			return CNTK::DeviceDescriptor::GPUDevice(0);
		}
	}

	bool GetInputVariableByName(CNTK::FunctionPtr evalFunc, std::wstring varName, CNTK::Variable& var)
	{
		auto variableLists = evalFunc->Arguments();

		for (std::vector<CNTK::Variable>::iterator it = variableLists.begin(); it != variableLists.end(); ++it)
		{
			if (it->Name().compare(varName) == 0)
			{
				var = *it;
				return true;
			}
		}
		return false;
	}

	std::wstring ConvertFString2WString(FString fstr)
	{
		std::string str = TCHAR_TO_UTF8(*fstr);
		std::wstring wstr = L"";
		wstr.assign(str.begin(), str.end());

		return wstr;
	}
};

struct FFunctionPtr
{
	FFunctionPtr()
	{}

	FFunctionPtr(CNTK::FunctionPtr InFunctionPtr)
		: functionPtr(InFunctionPtr)
	{
	}
	CNTK::FunctionPtr functionPtr;
};

struct FEvaluationMap
{
	FEvaluationMap()
	{}

	FEvaluationMap(std::unordered_map<CNTK::Variable, CNTK::ValuePtr> InMap)
		: map(InMap)
	{
	}
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> map;
};

struct FPrivateTensorflow
{
	FPrivateTensorflow()
	{}

	FPrivateTensorflow(dl_tensorflow::ITensorflow* InSession)
		: session(InSession)
	{
	}

	~FPrivateTensorflow()
	{
		session->Destroy();
	}

	dl_tensorflow::ITensorflow* session;
};

FCNTKFunction UDeepFunctionLibrary::GetCNTKFunction(ECNTKDeviceType DeviceType, FString modelPath)
{
	FCNTKFunction Function;

	std::wstring path = CNTKHelper::ConvertFString2WString(modelPath);
	auto device = CNTKHelper::GetDeviceDescriptor(DeviceType);

	Function.Handle = MakeShareable(new FFunctionPtr(CNTK::Function::Load(
		CNTKHelper::ConvertFString2WString(modelPath),
		CNTKHelper::GetDeviceDescriptor(DeviceType)
	)));

	return Function;
}

FCNTKVariables UDeepFunctionLibrary::GetCNTKVariables(FCNTKFunction Function, ECNTKVariableType VariableType)
{
	FCNTKVariables Out;
	std::vector<CNTK::Variable> Variables;

	if (VariableType == ECNTKVariableType::INPUT)
	{
		Variables = Function.Handle->functionPtr->Arguments();
	}
	else
	{
		Variables = Function.Handle->functionPtr->Outputs();
	}

	for (std::vector<CNTK::Variable>::iterator it = Variables.begin(); it != Variables.end(); ++it)
	{
		FCNTKVariable CNTKVariable;

		CNTKVariable.Name = UTF8_TO_TCHAR(it->Name().c_str());
		CNTKVariable.Kind = static_cast<int>(it->Kind());
		CNTKVariable.ShapeSize = static_cast<int>(it->Shape().TotalSize());

		Out.Variables.Add(CNTKVariable);
	}

	return Out;
}

FCNTKEvaluateMap UDeepFunctionLibrary::GetEmptyEvaluateMap()
{
	FCNTKEvaluateMap Map;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> emptyMap;
	Map.Handle = MakeShareable(new FEvaluationMap(emptyMap));

	return Map;
}

FCNTKEvaluateMap UDeepFunctionLibrary::GetEvaluateMap(FString varName, ECNTKDeviceType DeviceType, FCNTKFunction Function)
{
	FCNTKEvaluateMap Map;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> emptyMap;
	Map.Handle = MakeShareable(new FEvaluationMap(emptyMap));

	CNTK::Variable Var;
	if (CNTKHelper::GetInputVariableByName(Function.Handle->functionPtr, CNTKHelper::ConvertFString2WString(varName), Var))
	{
		CNTK::ValuePtr VarPtr;
		Map.Handle->map.insert({ Var, VarPtr });
	}

	return Map;
}

void UDeepFunctionLibrary::LoadEmptyEvaluateMap(FCNTKEvaluateMap Map, ECNTKDeviceType DeviceType, FCNTKFunction Function, FString varName, TArray<float> varValues)
{
	CNTK::Variable Var;
	if (CNTKHelper::GetInputVariableByName(Function.Handle->functionPtr, CNTKHelper::ConvertFString2WString(varName), Var))
	{
		std::vector<float> batchData;
		for (auto varValue : varValues)
		{
			batchData.push_back(varValue);
		}
		
		CNTK::ValuePtr VarPtr = CNTK::Value::CreateBatch(Var.Shape(), batchData, CNTKHelper::GetDeviceDescriptor(DeviceType));

		Map.Handle->map.clear();
		Map.Handle->map.insert({ Var, VarPtr });
	}
}

void UDeepFunctionLibrary::LoadEvaluteMap(FCNTKEvaluateMap Map, ECNTKDeviceType DeviceType, FCNTKFunction Function, TArray<float> varValues)
{
	if (Map.Handle->map.size() > 0)
	{
		auto it = Map.Handle->map.begin();
		CNTK::Variable Var = it->first;

		std::vector<float> batchData;
		for (auto varValue : varValues)
		{
			batchData.push_back(varValue);
		}

		it->second = CNTK::Value::CreateBatch(Var.Shape(), batchData, CNTKHelper::GetDeviceDescriptor(DeviceType));
	}
}

FCNTKOutput UDeepFunctionLibrary::RunCNTK(const FCNTKFunction& Function, const FCNTKEvaluateMap& arguments)
{
	FCNTKOutput Output;

	CNTK::ValuePtr outputValue;
	CNTK::Variable outputVar = Function.Handle->functionPtr->Output();
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputVar, outputValue } };

	Function.Handle->functionPtr->Evaluate(arguments.Handle->map, outputs);
	
	std::vector<std::vector<float>> outputData;
	outputValue = outputs[outputVar];
	outputValue->CopyVariableValueTo(outputVar, outputData);
	
	for (size_t i = 0; i < outputData.size(); i++)
	{
		FCNTKDim Dim;
		for (size_t j = 0; j < outputData[i].size(); j++)
		{
			Dim.Values.Add(outputData[i][j]);
		}
		Output.Dims.Add(MoveTemp(Dim));
	}
	
	return Output;
}

std::vector<std::vector<float>> UDeepFunctionLibrary::RunFast(const FCNTKFunction& Function, const FCNTKEvaluateMap& arguments)
{
	CNTK::ValuePtr outputValue;
	CNTK::Variable outputVar = Function.Handle->functionPtr->Output();
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputVar, outputValue } };

	Function.Handle->functionPtr->Evaluate(arguments.Handle->map, outputs);

	std::vector<std::vector<float>> outputData;
	outputValue = outputs[outputVar];
	outputValue->CopyVariableValueTo(outputVar, outputData);

	return outputData;
}

FTensorflowSession UDeepFunctionLibrary::CreateSession(FString GraphDef)
{
	FTensorflowSession Out;
	Out.Handle = MakeShareable(new FPrivateTensorflow(CreateTensorflow(TCHAR_TO_ANSI(*GraphDef))));
	return Out;
}

TArray<FString> UDeepFunctionLibrary::GetTSVariables(FTensorflowSession Session)
{
	TArray<FString> Out;

	auto names = Session.Handle->session->GetInputNames();
	for (auto name : names)
	{
		Out.Add(ANSI_TO_TCHAR(name.c_str()));
	}

	return Out;
}

FName UDeepFunctionLibrary::GetStatus(FTensorflowSession Session)
{
	return FName(Session.Handle->session->GetStatus().c_str());
}

void UDeepFunctionLibrary::PushInput(FTensorflowSession Session, FString Name, ETensorflowDataType DataType, const TArray<int32>& Dimension)
{
	Session.Handle->session->PushInput(
		TCHAR_TO_ANSI(*Name),
		(dl_tensorflow::DataType)DataType,
		Dimension.Num() > 0 ? Dimension[0] : 0,
		Dimension.Num() > 1 ? Dimension[1] : 0,
		Dimension.Num() > 2 ? Dimension[2] : 0,
		Dimension.Num() > 3 ? Dimension[3] : 0,
		Dimension.Num() > 4 ? Dimension[4] : 0
		);
}

void UDeepFunctionLibrary::LoadInput(FTensorflowSession Session, FString Name)
{
	Session.Handle->session->LoadInput(TCHAR_TO_ANSI(*Name));
}

void UDeepFunctionLibrary::LoadOutput(FTensorflowSession Session, FString Name)
{
	Session.Handle->session->LoadOutput(TCHAR_TO_ANSI(*Name));
}

int32 UDeepFunctionLibrary::NumElements(FTensorflowSession Session)
{
	return Session.Handle->session->GetNumElements();
}

void UDeepFunctionLibrary::RunTS(FTensorflowSession Session, const TArray<FString>& Names)
{
	Session.Handle->session->Run(
		TCHAR_TO_ANSI(*Names[0]),
		Names.Num() > 1 ? TCHAR_TO_ANSI(*Names[1]) : nullptr,
		Names.Num() > 2 ? TCHAR_TO_ANSI(*Names[2]) : nullptr,
		Names.Num() > 3 ? TCHAR_TO_ANSI(*Names[3]) : nullptr,
		Names.Num() > 4 ? TCHAR_TO_ANSI(*Names[4]) : nullptr
		);
} 

bool UDeepFunctionLibrary::Read(FTensorflowSession& Session, ETensorflowDataType Type)
{
	auto native_type = (dl_tensorflow::DataType)Type;
	if (Type == ETensorflowDataType::DT_FLOAT)
	{
		auto Size = NumElements(Session);
		Session.FlatFloat.Reset(Size);
		Session.FlatFloat.AddUninitialized(Size);
		return Session.Handle->session->Read(native_type, Session.FlatFloat.GetData());
	}
	
	return false;
}

bool UDeepFunctionLibrary::Write(FTensorflowSession& Session, ETensorflowDataType Type)
{
	auto native_type = (dl_tensorflow::DataType)Type;
	if (Type == ETensorflowDataType::DT_FLOAT)
	{
		auto Size = NumElements(Session);
		if (Session.FlatFloat.Num() != Size) 
		{
			return false;
		}
		return Session.Handle->session->Write(native_type, Session.FlatFloat.GetData());
	}

	return false;
}

bool UDeepFunctionLibrary::TestCNTK()
{
	// auto func = UDeepFunctionLibrary::GetCNTKFunction(ECNTKDeviceType::CPU, "C:\\something.pretrained");
	// auto map = UDeepFunctionLibrary::GetEmptyEvaluateMap();
	// TArray<float> input = { 0., 0., 1., 0., 0., 1., 1., 1. };

	// UDeepFunctionLibrary::LoadEmptyEvaluateMap(map, ECNTKDeviceType::CPU, func, "", input);

	// auto output = UDeepFunctionLibrary::RunCNTK(func, map);
	// output = { 0, 1, 1, 0 }
	return true;
}

bool UDeepFunctionLibrary::TestTensorflow()
{
	// FString GraphDef = "C:\\something.pb";

	// FTensorflowSession session = UDeepFunctionLibrary::CreateSession(GraphDef);
	// if(!session.Handle.IsValid())
	// {
	// 	return succ = false;
	// }
		
	// UDeepFunctionLibrary::PushInput(session, FString(TEXT("a")), ETensorflowDataType::DT_FLOAT, { 1 });
	// UDeepFunctionLibrary::PushInput(session, FString(TEXT("b")), ETensorflowDataType::DT_FLOAT, { 1 });
	
	// UDeepFunctionLibrary::LoadInput(session, FString(TEXT("a")));
	// session.FlatFloat.Reset();
	// session.FlatFloat.Add(3.0f);
	// if (!UDeepFunctionLibrary::Write(session, ETensorflowDataType::DT_FLOAT))
	// {
	// 	return succ = false;
	// }

	// UDeepFunctionLibrary::LoadInput(session, FString(TEXT("b")));
	// session.FlatFloat.Reset();
	// session.FlatFloat.Add(2.0f);
	// if (!UDeepFunctionLibrary::Write(session, ETensorflowDataType::DT_FLOAT))
	// {
	// 	return succ = false;
	// }

	// UDeepFunctionLibrary::RunTS(session, { FString(TEXT("c")) });

	// if (UDeepFunctionLibrary::GetStatus(session) != "OK")
	// {
	// 	return succ = false;
	// }

	// UDeepFunctionLibrary::LoadOutput(session, FString(TEXT("c")));
	// if (!UDeepFunctionLibrary::Read(session, ETensorflowDataType::DT_FLOAT))
	// {
	// 	return succ = false;
	// }

	// auto output = session.FlatFloat;
	// output = [6.0]
	return true;
}

IMPLEMENT_MODULE(FDeepLibrary, DeepLibrary)