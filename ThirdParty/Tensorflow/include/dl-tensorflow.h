#pragma once

#include <vector>
#include <string.h>

#ifdef EXPORT_DLTENSORFLOW
#	define DLTENSORFLOW_API __declspec(dllexport)
#else
#	define DLTENSORFLOW_API __declspec(dllimport)
#endif

namespace dl_tensorflow {
	enum DataType {
		DT_INVALID = 0,
		DT_FLOAT = 1,
		DT_DOUBLE = 2,
		DT_INT32 = 3,
		DT_UINT8 = 4,
		DT_INT16 = 5,
		DT_INT8 = 6,
		DT_STRING = 7,
		DT_COMPLEX64 = 8,
		DT_INT64 = 9,
		DT_BOOL = 10,
		DT_QINT8 = 11,
		DT_QUINT8 = 12,
		DT_QINT32 = 13,
		DT_BFLOAT16 = 14,
		DT_QINT16 = 15,
		DT_QUINT16 = 16,
		DT_UINT16 = 17,
		DT_COMPLEX128 = 18,
		DT_HALF = 19,
		DT_RESOURCE = 20,
		DT_FLOAT_REF = 101,
		DT_DOUBLE_REF = 102,
		DT_INT32_REF = 103,
		DT_UINT8_REF = 104,
		DT_INT16_REF = 105,
		DT_INT8_REF = 106,
		DT_STRING_REF = 107,
		DT_COMPLEX64_REF = 108,
		DT_INT64_REF = 109,
		DT_BOOL_REF = 110,
		DT_QINT8_REF = 111,
		DT_QUINT8_REF = 112,
		DT_QINT32_REF = 113,
		DT_BFLOAT16_REF = 114,
		DT_QINT16_REF = 115,
		DT_QUINT16_REF = 116,
		DT_UINT16_REF = 117,
		DT_COMPLEX128_REF = 118,
		DT_HALF_REF = 119,
		DT_RESOURCE_REF = 120
	};
	struct ITensorflow
	{		
		virtual void Destroy() = 0;
		virtual const std::vector<std::string>& GetInputNames() = 0;
		virtual const std::string& GetStatus() = 0;
		virtual void PushInput(const char* Name, dl_tensorflow::DataType Type, unsigned int Dim0, unsigned int Dim1 = 0, unsigned int Dim2 = 0, unsigned int Dim3 = 0, unsigned int Dim4 = 0) = 0;
		virtual void Run(const char* Out1, const char* Out2 = nullptr, const char* Out3 = nullptr, const char* Out4 = nullptr, const char* Out5 = nullptr) = 0;
		virtual void LoadInput(const char* Name) = 0;
		virtual void LoadOutput(const char* Name) = 0;
		virtual unsigned int GetNumElements() = 0;
		virtual bool Read(dl_tensorflow::DataType Type, void* Buffer) = 0;
		virtual bool Write(dl_tensorflow::DataType Type, const void* Buffer) = 0;
	};
}

DLTENSORFLOW_API dl_tensorflow::ITensorflow* CreateTensorflow(const char* BinaryProtoFile);