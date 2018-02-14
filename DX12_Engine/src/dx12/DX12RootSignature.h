// root signature management and helper
// manage default parameters for the pipeline state object

#pragma once

#include "dx12/d3dx12.h"
#include <vector>

class DX12RootSignature
{
public:
	DX12RootSignature();
	~DX12RootSignature();

	enum ERootParamType
	{
		eConstantBuffer,
		eShaderResourceView,
		eTableDescriptor,
		eUnorderedAccessView,
		eNone,
	};

	// root signature creation
	void		CreateDefaultRootSignature();	// create the default signature (add basic buffer as transform and global buffer to the root signature)
	// add parameters to the root signature (Warning parameters are sorted on the entry you put)
	void		AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC & i_Sampler);
	void		AddShaderResourceView(UINT32 i_ShaderRegister, UINT32 i_Register /* t0 to t7*/, D3D12_SHADER_VISIBILITY i_Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT32 i_RegisterSpace = 0);
	void		AddConstantBuffer(UINT32 i_ShaderRegister, UINT32 i_Register /* b0 to b7*/, D3D12_SHADER_VISIBILITY i_Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT32 i_RegisterSpace = 0);
	void		AddDescriptorRange(UINT32 i_ShaderRegister, UINT32 i_Register, const D3D12_DESCRIPTOR_RANGE * i_RangeTable, UINT32 i_RangeSize, D3D12_SHADER_VISIBILITY i_Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT32 i_RegisterSpace = 0);

	// create the root signature on the device
	HRESULT		Create(ID3D12Device * i_Device, 
		D3D12_ROOT_SIGNATURE_FLAGS i_Flags = 
		  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	// dx12
	ID3D12RootSignature *		GetRootSignature() const;

	// information
	bool		IsCreated() const;
	UINT		GetParamCount() const;
	UINT		GetStaticSamplerCount() const;
	// register management
	bool		IsRegisterFilled(const char * i_Register) const;
	bool		IsRegisterFilled(D3D12_ROOT_PARAMETER_TYPE  i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);
	bool		IsRegisterFilled(D3D12_DESCRIPTOR_RANGE_TYPE  i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);
	UINT		GetRegisterParamIndex(const char * i_Register) const;
	UINT		GetRegisterParamIndex(D3D12_ROOT_PARAMETER_TYPE  i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);
	UINT		GetRegisterParamIndex(D3D12_DESCRIPTOR_RANGE_TYPE  i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);

private:
	// internal helper
	D3D12_ROOT_DESCRIPTOR		CreateRootDescriptor(UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);
	bool						RegisterParameter(const D3D12_ROOT_PARAMETER & i_Parameter);
	void						GenerateBufferId(std::string & o_Buffer, D3D12_ROOT_PARAMETER_TYPE i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);
	void						GenerateBufferId(std::string & o_Buffer, D3D12_DESCRIPTOR_RANGE_TYPE  i_Type, UINT32 i_ShaderRegister, UINT32 i_ShaderSpace);

	std::vector<std::string>		m_RegisterBinded;

	// dx12 parameters
	std::vector<D3D12_ROOT_DESCRIPTOR_TABLE>	m_DescriptorTable;	// descriptor table that contains descriptor ranges
	std::vector<D3D12_STATIC_SAMPLER_DESC>		m_StaticSampler;	// static sampler for the root signature
	std::vector<D3D12_ROOT_PARAMETER>			m_RootParameters;	// parameters for the root (computed when create root)
	// internal management
	std::vector<D3D12_ROOT_DESCRIPTOR>		m_RootDesc;	// already registered descriptors
	// dx12
	ID3D12RootSignature *				m_RootSignature;

	// manager
	std::vector<D3D12_DESCRIPTOR_RANGE>		m_DescriptorRange;

	// internal
	bool		m_IsCreated;
};
