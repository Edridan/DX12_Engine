// this store macros and utils helpers

#pragma once

#include "d3dx12.h"
#include <DirectXMath.h>
#include <Windows.h>

// release and delete macros
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#define ENABLE_DEBUG_BREAK 1

#if (ENABLE_DEBUG_BREAK)
#define DEBUG_BREAK		__debugbreak()
#else
#define DEBUG_BREAK		void(0)
#endif


// Type definition
typedef UINT64 ADDRESS_ID;

const ADDRESS_ID UnavailableAdressId = ((ADDRESS_ID)-1);

// math
const DirectX::XMFLOAT3 Vec3Zero	= { 0.f, 0.f, 0.f };
const DirectX::XMFLOAT3 Vec3One		= { 1.f, 1.f, 1.f };
const DirectX::XMFLOAT3 Vec3Up		= { 0.f, 1.f, 0.f };
const DirectX::XMFLOAT3 Vec3Right	= { 1.f, 0.f, 0.f };

const FLOAT Zero	= 0.f;
const FLOAT One		= 1.f;

// struct
struct IntVec2
{
	int x, y;
	IntVec2() :x(0), y(0) {};
	IntVec2(int i_X, int i_Y) :x(i_X), y(i_Y) {};
};


// Helpers
// return the size of an DXGI_FORMAT element
UINT SizeOfFormatElement(DXGI_FORMAT i_Format);