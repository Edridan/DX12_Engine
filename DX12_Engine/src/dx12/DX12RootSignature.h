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

	// root signature creation
	void		CreateDefaultRootSignature();	// create the default signature (add basic buffer as transform and global buffer to the root signature)
	void		AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC & i_Sampler);
	void		AddParameter();
	void		PushRangeTable();	// call this function to start a range table
	void		AddRangedParameter(const D3D12_DESCRIPTOR_RANGE & i_Desc);	// add parameters of the current ranged table
	void		PopRangeTable();	// call this function when the range table is finished

	// create the root signature on the device
	HRESULT		Create(ID3D12Device * i_Device, 
		D3D12_ROOT_SIGNATURE_FLAGS i_Flags = 
		  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	// information
	bool		IsCreated() const;
	UINT		GetParamCount() const;
	UINT		GetStaticSamplerCount() const;
private:
	// dx12 parameters
	std::vector<D3D12_ROOT_PARAMETER>			m_RootParameters;	// parameters for the root
	std::vector<D3D12_ROOT_DESCRIPTOR_TABLE>	m_DescriptorTable;	// descriptor table that contains descriptor ranges
	std::vector<D3D12_STATIC_SAMPLER_DESC>		m_StaticSampler;	// static sampler for the root signature

	// manager
	std::vector<D3D12_DESCRIPTOR_RANGE>		m_DescriptorRange;

	// internal
	bool		m_IsCreated;
	UINT		m_ParamCount;
	UINT		m_SamplerCount;
};
