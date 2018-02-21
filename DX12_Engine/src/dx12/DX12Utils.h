// this store macros and utils helpers

#pragma once

#include "dx12/d3dx12.h"
#include "engine/Debug.h"	// DX12_ASSERT
#include "engine/Utils.h"
#include <DirectXMath.h>
#include <wincodec.h>

// macros definition
#define DEBUG_DX12_ENABLE		1

// debug management
#if (DEBUG_DX12_ENABLE) && defined(_DEBUG)
#define DX12_DEBUG
#endif


// release and delete macros
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }

// Type definition
typedef UINT64 ADDRESS_ID;
const ADDRESS_ID UnavailableAdressId = ((ADDRESS_ID)-1);

// math
// define values
const DirectX::XMFLOAT3 Vec3Zero	= { 0.f, 0.f, 0.f };
const DirectX::XMFLOAT3 Vec3One		= { 1.f, 1.f, 1.f };
const DirectX::XMFLOAT3 Vec3Up		= { 0.f, 1.f, 0.f };
const DirectX::XMFLOAT3 Vec3Right	= { 1.f, 0.f, 0.f };

const DirectX::XMFLOAT4 Vec4Zero	= { 0.f, 0.f, 0.f, 0.f };
const DirectX::XMFLOAT4 Vec4One		= { 1.f, 1.f, 1.f, 1.f };
const DirectX::XMFLOAT4 Vec4Up		= { 0.f, 1.f, 0.f, 1.f };
const DirectX::XMFLOAT4 Vec4Right	= { 1.f, 0.f, 0.f, 1.f };

// Helpers
// return the size of an DXGI_FORMAT element
UINT SizeOfFormatElement(DXGI_FORMAT i_Format);
// get the dxgi format equivalent of a wic format
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID & i_WicFormat);
// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID & i_WicFormat);
// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(const DXGI_FORMAT & i_DxGIFormat);


// color management
inline DirectX::XMFLOAT3 ColorToVec3(const Color & i_Color)
{
	return DirectX::XMFLOAT3(i_Color.r, i_Color.g, i_Color.b);
}

inline DirectX::XMFLOAT4 ColorToVec4(const Color & i_Color)
{
	return DirectX::XMFLOAT4(i_Color.r, i_Color.g, i_Color.b, 1.f);
}

#ifdef _DEBUG
// assert on DX12 error
#define DX12_ASSERT(i_Hres)											\
do {																\
	if (FAILED(i_Hres))												\
	{																\
		PRINT_DEBUG("%s : %s %i", #i_Hres, __FILE__, __LINE__);		\
		POPUP_ERROR("%s : %s %i", #i_Hres, __FILE__, __LINE__);		\
		DEBUG_BREAK;												\
	}																\
} while(false)
#else
#define DX12_ASSERT(i_Hres)		i_Hres
#endif