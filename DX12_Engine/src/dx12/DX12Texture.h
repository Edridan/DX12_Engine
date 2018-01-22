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

	DX12Texture(const wchar_t * i_Filename);
	~DX12Texture();

	// information
	IntVec2					GetSize() const;
	const std::wstring &	GetName() const;
	bool					IsLoaded() const;

	// dx12 management
	const ID3D12DescriptorHeap *	GetDescriptorHeap() const;
	HRESULT							PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);
	
private:
	// load image data helper
	static int		LoadImageDataFromFile(BYTE** o_ImageData, D3D12_RESOURCE_DESC & o_ResourceDescription, ImageDataDesc & o_Desc, LPCWSTR i_Filename);

	// information
	D3D12_RESOURCE_DESC		m_Desc;
	ID3D12Resource *		m_TextureBuffer;
	ID3D12Resource *		m_TextureBufferUploadHeap;
	ID3D12DescriptorHeap *	m_DescriptorHeap;

	std::wstring		m_Name;
	bool				m_IsLoaded;
};