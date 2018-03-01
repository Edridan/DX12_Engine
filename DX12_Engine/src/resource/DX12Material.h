// material that contains data to render properly objects (color, textures...)
#pragma once

// class predef
class DX12Texture;
class DX12RootSignature;
class DX12PipelineState;

#include "DX12Resource.h"
#include "dx12/DX12Utils.h"
#include "dx12/DX12Shader.h"
#include "dx12/DX12ConstantBuffer.h"
#include <string>

class DX12Material : public DX12Resource
{
public:
	struct DX12MaterialData
	{
		// material data
		Color Ka = color::Pink, Kd = color::Pink, Ks = color::Pink, Ke = color::Pink;
		float Ns = 1000.f;
		DX12Texture *	map_Kd = nullptr, *map_Ks = nullptr, *map_Ka = nullptr;
		// data info
		std::string	Name, Filepath;
	};

	// destructor
	~DX12Material();

	// dx12 management
	void		PushPipelineState(ID3D12GraphicsCommandList * i_CommandList) const;
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList, UINT i_RootParameter = 2 /* Root parameter index (basically 2 but can be changed) */) const;
	void		UpdateConstantBuffer() const;	// this update constant buffer for Shader buffer

	friend class DX12ResourceManager;
private:
	DX12Material();

	// internal helper
	void		GenerateRootSignature(ID3D12Device * i_Device);
	void		GeneratePipelineState(ID3D12Device * i_Device);

	// define data for material
	__declspec(align(16)) struct MaterialData
	{
		DirectX::XMFLOAT4		Ka, Kd, Ks, Ke;
		BOOL					Map_A, Map_D, Map_S;
		float					Ns;
	};

	// Inherited via DX12Resource
	virtual void LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) override;
	virtual void Release() override;

	// pipeline state object
	DX12RootSignature *		m_RootSignature;
	DX12PipelineState *		m_PipelineState;

	// external
	DX12ConstantBuffer *	m_ConstantBuffer;

	// material specs
	ADDRESS_ID				m_BufferAddress;
	MaterialData			m_Data;	// data sended to the GPU
};