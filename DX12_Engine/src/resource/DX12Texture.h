#pragma once

#include "DX12Resource.h"
#include "engine/Utils.h"
#include "dx12/d3dx12.h"

class DX12Texture : public DX12Resource
{
public:
	struct DX12TextureData
	{
		// default image data
		int				Width, Height;	// size of the image
		DXGI_FORMAT		Format		= DXGI_FORMAT_R8G8B8A8_UNORM;	// format
		BYTE *			ImageData = nullptr;	// image pixels data
		// other
		std::string		Name, Filepath;
	};

	// destructor
	~DX12Texture();

	// information
	DXGI_FORMAT					GetFormat() const;
	IntVec2						GetSize() const;
	D3D12_GPU_DESCRIPTOR_HANDLE	GetGPUDescriptorHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE	GetCPUDescriptorHandle() const;

	// friend class
	friend class DX12ResourceManager;

private:
	DX12Texture();

	// Inherited via DX12Resource
	virtual void	LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) override;
	virtual void	PreloadData(const void * i_Data) override;
	virtual void	Release() override;

	// helpers
	HRESULT			CreateResourceBuffer(ID3D12Device * i_Device, const std::wstring & i_BufferName);
	HRESULT			CreateUploadBuffer(ID3D12Device * i_Device, const std::wstring & i_BufferName);

	// dx12
	D3D12_RESOURCE_DESC		m_Desc;
	ID3D12Resource *		m_ResourceBuffer;
	ID3D12Resource *		m_UploadBuffer;
	ID3D12DescriptorHeap *	m_DescriptorHeap;
};