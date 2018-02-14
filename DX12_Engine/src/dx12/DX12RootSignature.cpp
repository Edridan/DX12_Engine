#include "DX12RootSignature.h"

#include "engine/Debug.h"
#include "dx12/DX12RenderEngine.h"

#define ASSERT_AND_EXIT(i_Condition)										\
do {																		\
	if (!(i_Condition))														\
	{																		\
		PRINT_DEBUG("%s assert an error file : %s, [%i]", #i_Condition,		\
				__FILE__, __LINE__);										\
		PopUpWindow(eError, "Assert", " %s Assert error file : %s [%i]",	\
				#i_Condition, __FILE__, __LINE__);							\
		DEBUG_BREAK;														\
		return;																\
	}																		\
} while (false)


DX12RootSignature::DX12RootSignature()
	:m_IsCreated(false)
	,m_RootSignature(nullptr)
{
}

DX12RootSignature::~DX12RootSignature()
{
	// clear resources
	for (UINT i = 0; i < m_RootParameters.size(); i++)
	{
		if (m_RootParameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete [] m_RootParameters[i].DescriptorTable.pDescriptorRanges;
		}
	}

	// clear vectors
	m_RootParameters.clear();
	m_DescriptorTable.clear();
	m_StaticSampler.clear();

	// clear dx12 resources
	if (m_IsCreated)
	{
		SAFE_RELEASE(m_RootSignature);
	}
}

void DX12RootSignature::CreateDefaultRootSignature()
{
	// bind buffer this way
	AddConstantBuffer(0, 0, D3D12_SHADER_VISIBILITY_ALL);		// [0] b0 CBV for global constants (time, camera position etc...)
	AddConstantBuffer(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);	// [1] b1 CBV for transform matrix
	AddConstantBuffer(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);	// [2] b2 CBV for material
}

void DX12RootSignature::AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC & i_Sampler)
{
	// push back a static sampler to the root signature
	m_StaticSampler.push_back(i_Sampler);
}

void DX12RootSignature::AddShaderResourceView(UINT32 i_ShaderRegister, UINT32 i_RegisterSpace, D3D12_SHADER_VISIBILITY i_Visibility)
{
	ASSERT_AND_EXIT(!m_IsCreated);
	

	// create the root descriptor
	D3D12_ROOT_DESCRIPTOR rootDesc = CreateRootDescriptor(i_ShaderRegister, i_RegisterSpace);

	// create the root parameter
	D3D12_ROOT_PARAMETER rootParam;
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParam.ShaderVisibility = i_Visibility;
	rootParam.Descriptor = rootDesc;

	// push the root parameter
	RegisterParameter(rootParam);
}

void DX12RootSignature::AddConstantBuffer(UINT32 i_ShaderRegister, UINT32 i_RegisterSpace, D3D12_SHADER_VISIBILITY i_Visibility)
{
	ASSERT(!m_IsCreated);

	// create the root descriptor
	D3D12_ROOT_DESCRIPTOR rootDesc = CreateRootDescriptor(i_ShaderRegister, i_RegisterSpace);

	// create the root parameter
	D3D12_ROOT_PARAMETER rootParam;
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam.ShaderVisibility = i_Visibility;
	rootParam.Descriptor = rootDesc;

	RegisterParameter(rootParam);
}

void DX12RootSignature::AddDescriptorRange(const D3D12_DESCRIPTOR_RANGE * i_RangeTable, UINT32 i_RangeSize, D3D12_SHADER_VISIBILITY i_Visibility)
{
	ASSERT(!m_IsCreated);

	// copy the array
	D3D12_DESCRIPTOR_RANGE * rangeTable = new D3D12_DESCRIPTOR_RANGE[i_RangeSize];
	// create the table descriptor
	D3D12_ROOT_DESCRIPTOR_TABLE tableDesc;
	tableDesc.NumDescriptorRanges = i_RangeSize;
	tableDesc.pDescriptorRanges = rangeTable;

	for (UINT i = 0; i < i_RangeSize; ++i)
	{
		// copy the range table in a local range table
		rangeTable[i] = i_RangeTable[i];
	}

	m_DescriptorTable.push_back(tableDesc);

	// create the root parameter
	D3D12_ROOT_PARAMETER rootParam;
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.ShaderVisibility = i_Visibility;
	rootParam.DescriptorTable = tableDesc;

	RegisterParameter(rootParam);
}

HRESULT DX12RootSignature::Create(ID3D12Device * i_Device, D3D12_ROOT_SIGNATURE_FLAGS i_Flags)
{
	ASSERT(!m_IsCreated);	// can't recreate an already created root signature
	ASSERT(m_RootParameters.size() > 0);

	HRESULT hr;
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();

	// create the root signature description from root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_ROOT_PARAMETER * rootParam			= nullptr;
	D3D12_STATIC_SAMPLER_DESC * staticSampler	= nullptr;

	if (m_RootParameters.size() > 0) rootParam	= &m_RootParameters[0];
	if (m_StaticSampler.size() > 0)	staticSampler = &m_StaticSampler[0];

	rootSignatureDesc.Init(
		(UINT)m_RootParameters.size(),	// number of parameters entry
		rootParam,		// a pointer to the beginning of our root parameters array
		(UINT)m_StaticSampler.size(),		// static samplers count
		staticSampler,		// static samplers pointer
		i_Flags						// flags
	);

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : D3D12SerializeRootSignature");
	}

	hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateRootSignature");
	}

	return hr;
}

ID3D12RootSignature * DX12RootSignature::GetRootSignature() const
{
	return m_RootSignature;
}

bool DX12RootSignature::IsCreated() const
{
	return m_IsCreated;
}

UINT DX12RootSignature::GetParamCount() const
{
	return (UINT)m_RootParameters.size();
}

UINT DX12RootSignature::GetStaticSamplerCount() const
{
	return (UINT)m_StaticSampler.size();
}

bool DX12RootSignature::IsRegisterFilled(const char * i_Register) const
{
	return GetRegisterParamIndex(i_Register) != (UINT(-1));
}

bool DX12RootSignature::IsRegisterFilled(D3D12_DESCRIPTOR_RANGE_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	std::string buffer;
	GenerateBufferId(buffer, i_Type, i_ShaderRegister, i_ShaderSpace);
	return IsRegisterFilled(buffer.c_str());
}

bool DX12RootSignature::IsRegisterFilled(D3D12_ROOT_PARAMETER_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	std::string buffer;
	GenerateBufferId(buffer, i_Type, i_ShaderRegister, i_ShaderSpace);
	return IsRegisterFilled(buffer.c_str());
}

UINT DX12RootSignature::GetRegisterParamIndex(const char * i_Register) const
{
	for (UINT i = 0; i < m_RegisterBinded.size(); ++i)
	{
		if (m_RegisterBinded[i].find(i_Register) != std::string::npos)	return i;
	}

	return (UINT(-1));	// didn't found
}

UINT DX12RootSignature::GetRegisterParamIndex(D3D12_ROOT_PARAMETER_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	std::string buffer;
	GenerateBufferId(buffer, i_Type, i_ShaderRegister, i_ShaderSpace);
	return GetRegisterParamIndex(buffer.c_str());
}

UINT DX12RootSignature::GetRegisterParamIndex(D3D12_DESCRIPTOR_RANGE_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	std::string buffer;
	GenerateBufferId(buffer, i_Type, i_ShaderRegister, i_ShaderSpace);
	return GetRegisterParamIndex(buffer.c_str());
}

FORCEINLINE D3D12_ROOT_DESCRIPTOR DX12RootSignature::CreateRootDescriptor(UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	D3D12_ROOT_DESCRIPTOR rootDesc;

	// Update the root desc and push it to the already binded roots
	rootDesc.RegisterSpace = i_ShaderSpace;
	rootDesc.ShaderRegister = i_ShaderRegister;

	return rootDesc;
}

FORCEINLINE bool DX12RootSignature::RegisterParameter(const D3D12_ROOT_PARAMETER & i_Parameter)
{
	std::string registers;

	// register parameter for each shader register in table
	if (i_Parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
	{
		// iterate throught the table and add the register
		D3D12_ROOT_DESCRIPTOR_TABLE tableDesc = i_Parameter.DescriptorTable;
		const D3D12_DESCRIPTOR_RANGE * desc = tableDesc.pDescriptorRanges;

		for (UINT i = 0; i < tableDesc.NumDescriptorRanges; ++i)
		{
			std::string buffer;
			GenerateBufferId(buffer, desc[i].RangeType, desc[i].BaseShaderRegister, desc[i].RegisterSpace);
			registers.append(buffer);
			registers.append(";");
		}
	}
	else
	{
		// push back the register
		GenerateBufferId(registers, i_Parameter.ParameterType, i_Parameter.Descriptor.ShaderRegister, i_Parameter.Descriptor.RegisterSpace);
	}

	// add the root parameter to the list
	m_RegisterBinded.push_back(registers);
	m_RootParameters.push_back(i_Parameter);
	return true;
}

FORCEINLINE void DX12RootSignature::GenerateBufferId(std::string & o_Buffer, D3D12_ROOT_PARAMETER_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	switch (i_Type)
	{
	case D3D12_ROOT_PARAMETER_TYPE_CBV:	o_Buffer = "b";	break;
	case D3D12_ROOT_PARAMETER_TYPE_SRV: o_Buffer = "t";	break;
	case D3D12_ROOT_PARAMETER_TYPE_UAV: o_Buffer = "u";	break;
	default:
		break;
	}

	char buff[3];

	// retreive buffer index
	_itoa_s(i_ShaderRegister, buff, 10);
	o_Buffer.append(buff);

	// retreive buffer space
	if (i_ShaderSpace != 0)
	{
		o_Buffer.append(":space");
		_itoa_s(i_ShaderSpace, buff, 10);
		o_Buffer.append(buff);
	}
}

FORCEINLINE void DX12RootSignature::GenerateBufferId(std::string & o_Buffer, D3D12_DESCRIPTOR_RANGE_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	switch (i_Type)
	{
	case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:		o_Buffer = "b";	break;
	case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:		o_Buffer = "t";	break;
	case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:		o_Buffer = "u";	break;
	case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:	o_Buffer = "s"; break;
	default:
		break;
	}

	char buff[3];

	// retreive buffer index
	_itoa_s(i_ShaderRegister, buff, 10);
	o_Buffer.append(buff);

	// retreive buffer space
	if (i_ShaderSpace != 0)
	{
		o_Buffer.append(":space");
		_itoa_s(i_ShaderSpace, buff, 10);
		o_Buffer.append(buff);
	}
}
