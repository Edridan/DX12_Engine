#pragma once

#include "d3dx12.h"
#include "dx12/DX12Utils.h"
#include <string>

class DX12Texture
{
public:
	// struct
	struct ImageDataDesc
	{
		int BitsPerPixel;
		int BytesPerRow;
		int ImageSize;
		int Width;
		int Height;
	};

	struct ImageDesc
	{
		// default image data
		int Width;
		int Height;
		std::wstring Name = L"Unnamed";	// texture name for debug purpose
		BYTE * Data = nullptr;	// data for the image (must be corresponding the size and format)

								// dx12 information
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	};

	DX12Texture(const wchar_t * i_Filename);
	DX12Texture(const ImageDesc & i_Desc);
	~DX12Texture();

	// information
	IntVec2					GetSize() const;
	const std::wstring &	GetName() const;
	bool					IsLoaded() const;
	DXGI_FORMAT				GetFormat() const;

	// dx12 management
	ID3D12DescriptorHeap *				GetDescriptorHeap() const;
	const ID3D12Resource *				GetBuffer() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE	GetDescriptorHandle() const;

private:
	// load image data helper
	static int		LoadImageDataFromFile(BYTE** o_ImageData, D3D12_RESOURCE_DESC & o_ResourceDescription, ImageDataDesc & o_Desc, LPCWSTR i_Filename);

	// dx12
	D3D12_RESOURCE_DESC		m_Desc;
	ID3D12Resource *		m_TextureBuffer;
	ID3D12Resource *		m_TextureBufferUploadHeap;
	ID3D12DescriptorHeap *	m_DescriptorHeap;
	DXGI_FORMAT				m_Format;

	// helpers
	HRESULT			CreateTextureBufferResourceHeap(ID3D12Device * i_Device, const std::wstring & i_BufferName);
	HRESULT			CreateTextureBufferUploadHeap(ID3D12Device * i_Device, const std::wstring & i_BufferName);

	std::wstring		m_Name;
	bool				m_IsLoaded;
};