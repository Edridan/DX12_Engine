#pragma once

#include "d3dx12.h"

class DX12Texture
{
public:
	DX12Texture(const wchar_t * i_Filename);
	~DX12Texture();



private:
	// load image data helper
	static int		LoadImageDataFromFile(BYTE** o_ImageData, D3D12_RESOURCE_DESC & o_ResourceDescription, LPCWSTR i_Filename, int & i_BytesPerRow);

};