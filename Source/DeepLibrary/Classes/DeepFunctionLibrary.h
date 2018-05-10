#pragma once

#include "CoreMinimal.h"
#include <algorithm>
#include <vector>
#include "DeepFunctionLibrary.generated.h"

struct FFunctionPtr;
struct FEvaluationMap;
struct FPrivateTensorflow;

UENUM()
enum class ECNTKDeviceType : uint8
{
	CPU = 0,
	GPU = 1
};

UENUM()
enum class ECNTKVariableType : uint8
{
	INPUT = 0,
	OUTPUT = 1
};

USTRUCT()
struct FCNTKVariable
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString Name;

	UPROPERTY()
		int Kind;

	UPROPERTY()
		int ShapeSize;
};

USTRUCT(BlueprintType)
struct FCNTKVariables
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<FCNTKVariable> Variables;
};

USTRUCT()
struct FCNTKDim
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<float> Values;
};

USTRUCT(BlueprintType)
struct FCNTKOutput
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<FCNTKDim> Dims;
};

USTRUCT(BlueprintType)
struct FCNTKFunction
{
	GENERATED_BODY()

public:
	TSharedPtr<FFunctionPtr> Handle;
};

USTRUCT(BlueprintType)
struct FCNTKEvaluateMap
{
	GENERATED_BODY()

public:
	TSharedPtr<FEvaluationMap> Handle;
};

UENUM()
enum class ETensorflowDataType : uint8
{
	DT_INVALID = 0,
	DT_FLOAT = 1,
	DT_DOUBLE = 2,
	DT_INT32 = 3,
	DT_UINT8 = 4,
	DT_INT16 = 5,
	DT_INT8 = 6,
	DT_STRING = 7
};

USTRUCT(BlueprintType)
struct FTensorflowSession
{
	GENERATED_BODY()

public:
	FTensorflowSession()
	{}

	UPROPERTY()
		TArray<float> FlatFloat;

	TSharedPtr<FPrivateTensorflow> Handle;
};

UCLASS(BlueprintType)
class DEEPLIBRARY_API UDeepFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* CNTK */
	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static FCNTKFunction GetCNTKFunction(ECNTKDeviceType DeviceType, FString modelPath);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static FCNTKVariables GetCNTKVariables(FCNTKFunction CNTKFunction, ECNTKVariableType VariableType);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static FCNTKEvaluateMap GetEmptyEvaluateMap();

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static FCNTKEvaluateMap GetEvaluateMap(FString varName, ECNTKDeviceType DeviceType, FCNTKFunction Function);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static void LoadEmptyEvaluateMap(FCNTKEvaluateMap Map, ECNTKDeviceType DeviceType, FCNTKFunction Function, FString varName, TArray<float> varValues);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static void LoadEvaluteMap(FCNTKEvaluateMap Map, ECNTKDeviceType DeviceType, FCNTKFunction Function, TArray<float> varValues);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static FCNTKOutput RunCNTK(const FCNTKFunction& Function, const FCNTKEvaluateMap& arguments);
	/* Tensorflow */
	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static FTensorflowSession CreateSession(FString GraphDef);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static TArray<FString> GetTSVariables(FTensorflowSession Session);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static FName GetStatus(FTensorflowSession Session);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static void PushInput(FTensorflowSession Session, FString Name, ETensorflowDataType DataType, const TArray<int32>& Dimension);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static void LoadInput(FTensorflowSession Session, FString Name);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static void LoadOutput(FTensorflowSession Session, FString Name);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static int32 NumElements(FTensorflowSession Session);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static void RunTS(FTensorflowSession Session, const TArray<FString>& Names);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static bool Read(FTensorflowSession& Session, ETensorflowDataType Type);

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static bool Write(FTensorflowSession& Session, ETensorflowDataType Type);

public:

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|CNTK")
	static bool TestCNTK();

	UFUNCTION(BlueprintCallable, Category = "DeepLibary|Tensorflow")
	static bool TestTensorflow();

private:

	static std::vector<std::vector<float>> RunFast(const FCNTKFunction& Function, const FCNTKEvaluateMap& arguments);

};