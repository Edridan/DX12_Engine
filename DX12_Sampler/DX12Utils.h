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


const FLOAT Zero	= 0.f;
const FLOAT One		= 1.f;

// Helpers
// return the size of an DXGI_FORMAT element
UINT SizeOfFormatElement(DXGI_FORMAT i_Format);