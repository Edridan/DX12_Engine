#include "DX12RootSignature.h"

#include "engine/Debug.h"

DX12RootSignature::DX12RootSignature()
	:m_IsCreated(false)
	,m_ParamCount(0)
{
}

DX12RootSignature::~DX12RootSignature()
{
}

void DX12RootSignature::CreateDefaultRootSignature()
{
}

void DX12RootSignature::AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC & i_Sampler)
{
	// push back a static sampler to the root signature
	m_StaticSampler.push_back(i_Sampler);
}

void DX12RootSignature::AddShaderResourceView(UINT32 i_ShaderRegister, UINT32 i_Register, D3D12_SHADER_VISIBILITY i_Visibility, UINT32 i_RegisterSpace)
{
	ASSERT(!m_IsCreated);

	// create the root descriptor
	D3D12_ROOT_DESCRIPTOR rootDesc = CreateRootDescriptor(i_ShaderRegister, i_RegisterSpace);

	// create the root parameter
	D3D12_ROOT_PARAMETER rootParam;
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParam.ShaderVisibility = i_Visibility;
	rootParam.Descriptor = rootDesc;
}

void DX12RootSignature::AddConstantBuffer(UINT32 i_ShaderRegister, UINT32 i_Register, D3D12_SHADER_VISIBILITY i_Visibility, UINT32 i_RegisterSpace)
{
	ASSERT(!m_IsCreated);

	// create the root descriptor
	D3D12_ROOT_DESCRIPTOR rootDesc = CreateRootDescriptor(i_ShaderRegister, i_RegisterSpace);

	// create the root parameter
	D3D12_ROOT_PARAMETER rootParam;
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam.ShaderVisibility = i_Visibility;
	rootParam.Descriptor = rootDesc;
}

void DX12RootSignature::AddDescriptorRange(UINT32 i_ShaderRegister, UINT32 i_Register, const D3D12_DESCRIPTOR_RANGE * i_RangeTable, UINT32 i_RangeSize, D3D12_SHADER_VISIBILITY i_Visibility, UINT32 i_RegisterSpace)
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
}

HRESULT DX12RootSignature::Create(ID3D12Device * i_Device, D3D12_ROOT_SIGNATURE_FLAGS i_Flags)
{
	ASSERT(!m_IsCreated);	// can't recreate an already created root signature

	HRESULT hr;


	return hr;
}

bool DX12RootSignature::IsCreated() const
{
	return m_IsCreated;
}

UINT DX12RootSignature::GetParamCount() const
{
	return m_ParamCount;
}

UINT DX12RootSignature::GetStaticSamplerCount() const
{
	return m_SamplerCount;
}

FORCEINLINE D3D12_ROOT_DESCRIPTOR DX12RootSignature::CreateRootDescriptor(UINT32 i_ShaderRegister, UINT32 i_ShaderSpace)
{
	D3D12_ROOT_DESCRIPTOR rootDesc;

	// Update the root desc and push it to the already binded roots
	rootDesc.RegisterSpace = i_ShaderSpace;
	rootDesc.ShaderRegister = i_ShaderRegister;

	return rootDesc;
}

bool DX12RootSignature::RegisterParameter(UINT i_ShaderRegister, UINT32 i_ShaderSpace, D3D12_ROOT_PARAMETER_TYPE i_Type)
{
	// To do : register param to buffer to get error (if 2 params on the same register)
	return false;
}

/*
// each elements are rendered need a position
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC desc;
	DX12Shader * pixelShader = nullptr, *vertexShader = nullptr;
	UINT textureCount = 0;
	UINT samplerCount = 0;
	UINT bufferCount = CBV_COUNT;	// default buffer count, this include the 2 constant buffer for the materials

	// sampler for textures
	D3D12_STATIC_SAMPLER_DESC	* sampler			= nullptr;
	D3D12_ROOT_DESCRIPTOR_TABLE * descriptorTable	= nullptr;

	// create input layout
	DX12Mesh::CreateInputLayoutFromFlags(desc, i_Flags);

	// layout order definition depending flags : 
	// 1 - Position
	// 2 - Normal
	// 3 - Texcoord
	// 4 - Color
	pixelShader		= GetShader(i_Flags, DX12Shader::ePixel);
	vertexShader	= GetShader(i_Flags, DX12Shader::eVertex);
	
	if (pixelShader == nullptr || vertexShader == nullptr)
	{
		PopUpWindow(PopUpIcon::eWarning, "Warning", "Trying to create a pipeline state but pixel or vertex shaders are not loaded");
		DEBUG_BREAK;
		return;
	}

	// create sampler for mesh buffer with tex coord
	// we are going to use static samplers that are samplers that we can't change when they are set to the pipeline state
	// this mean we have to save samplers in files and read them when loading the mesh
	// To do : multiple textures count
	if (i_Flags & DX12Mesh::EElementFlags::eHaveTexcoord)
	{
		// at least one texture
		textureCount = 2;	// for now only ambient texture is managed
		samplerCount = 1;	// only one sample to manage all texture right now
		bufferCount += textureCount;		// add a descriptor table for each textures

		// create descriptor table ranges
		D3D12_DESCRIPTOR_RANGE *  descriptorTableRanges = new D3D12_DESCRIPTOR_RANGE[textureCount];
		descriptorTable = new D3D12_ROOT_DESCRIPTOR_TABLE[1];	// create a descriptor table

		for (UINT i = 0; i < textureCount; ++i)
		{
			descriptorTableRanges[i].RangeType			= D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
			descriptorTableRanges[i].NumDescriptors		= 1; // we only have one texture right now, so the range is only 1
			descriptorTableRanges[i].BaseShaderRegister = i; // start index of the shader registers in the range
			descriptorTableRanges[i].RegisterSpace		= 0; // space 0. can usually be zero
			descriptorTableRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
		}

		// descriptor table
		descriptorTable[0].NumDescriptorRanges	= textureCount;	// texture count
		descriptorTable[0].pDescriptorRanges	= descriptorTableRanges; // the pointer to the beginning of our ranges array

		sampler = new D3D12_STATIC_SAMPLER_DESC[samplerCount]; // create a descriptor table

		for (UINT i = 0; i < samplerCount; ++i)
		{
			sampler[0].Filter			= D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler[0].AddressU			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].AddressV			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].AddressW			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler[0].MipLODBias		= 0;
			sampler[0].MaxAnisotropy	= 0;
			sampler[0].ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
			sampler[0].BorderColor		= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler[0].MinLOD			= 0.0f;
			sampler[0].MaxLOD			= D3D12_FLOAT32_MAX;
			sampler[0].ShaderRegister	= 0;
			sampler[0].RegisterSpace	= 0;
			sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}
	}

	// -- Create root signature -- //

	// Constant buffer for materials and transform
	// create the root descriptor : where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR * rootCBVDescriptor = new D3D12_ROOT_DESCRIPTOR[CBV_COUNT];
	// transform constant buffer (matrix)
	rootCBVDescriptor[0].RegisterSpace = 0;
	rootCBVDescriptor[0].ShaderRegister = 0;
	// material constant buffer
	rootCBVDescriptor[1].RegisterSpace = 0;
	rootCBVDescriptor[1].ShaderRegister = 1;

	// create the default root parameter and fill it out
	// this paramater is the model view projection matrix
	D3D12_ROOT_PARAMETER *  rootParameters = new D3D12_ROOT_PARAMETER[bufferCount];  // constant buffer plus table range for textures

	static const D3D12_SHADER_VISIBILITY shaderVisibility[CBV_COUNT] =
	{
		D3D12_SHADER_VISIBILITY_VERTEX,
		D3D12_SHADER_VISIBILITY_PIXEL,
	};

	// material textures
	static D3D12_SHADER_VISIBILITY * textureShaderVisibility = new D3D12_SHADER_VISIBILITY[textureCount];
	if (textureCount > 0)	textureShaderVisibility[0] = D3D12_SHADER_VISIBILITY_PIXEL;	// ambient texture
	if (textureCount > 1)	textureShaderVisibility[1] = D3D12_SHADER_VISIBILITY_PIXEL;	// diffuse texture
	if (textureCount > 2)	textureShaderVisibility[2] = D3D12_SHADER_VISIBILITY_PIXEL;	// specular texture

	for (UINT i = 0; i < CBV_COUNT; ++i)
	{
		// first parameter is always the CBV
		rootParameters[i].ParameterType		= D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
		rootParameters[i].Descriptor		= rootCBVDescriptor[i]; // this is the root descriptor for this root parameter
		rootParameters[i].ShaderVisibility	= shaderVisibility[i]; // our vertex shader will be the only shader accessing this parameter for now
	}

	// setup the root parameters for textures
	if (textureCount != 0)
	{

	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	if (!(i_Flags & DX12Mesh::EElementFlags::eHaveTexcoord))
	{
		rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		bufferCount,				// number of buffer entry
		rootParameters,				// a pointer to the beginning of our root parameters array
		samplerCount,				// static samplers count
		sampler,					// static samplers pointer
		rootSignatureFlags			// flags
	);		

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : D3D12SerializeRootSignature");
	}

	ID3D12RootSignature * rootSignature = nullptr;
	hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	if (FAILED(hr))
	{
		ASSERT_ERROR("Error : CreateRootSignature");
	}
*/