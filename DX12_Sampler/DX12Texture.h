#pragma once

#include "d3dx12.h"
#include "DX12Utils.h"
#include <string>


class DX12Texture
{
public:
	DX12Texture(const wchar_t * i_Filename);
	~DX12Texture();

	IntVec2					GetSize() const;
	const std::wstring &	GetName() const;
	
private:
	// load image data helper
	static int		LoadImageDataFromFile(BYTE** o_ImageData, D3D12_RESOURCE_DESC & o_ResourceDescription, int & o_BytesPerRow, LPCWSTR i_Filename);

	// information
	D3D12_RESOURCE_DESC m_Desc;
	std::wstring		m_Name;
	bool				m_IsLoaded;
};