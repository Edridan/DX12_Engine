// this store macros and utils helpers

#pragma once

#include "d3dx12.h"
#include <DirectXMath.h>
#include <wincodec.h>

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


const float DegToRad = 0.01745329238f; // retreived from google
const float RadToDeg = 57.29578f;

// struct
struct IntVec2
{
	int x, y;
	IntVec2() :x(0), y(0) {};
	IntVec2(int i_X, int i_Y) :x(i_X), y(i_Y) {};

	// operators
	IntVec2 operator+(const IntVec2 & i_Other)
	{
		return IntVec2(x + i_Other.x, y + i_Other.y);
	}

	IntVec2 operator-(const IntVec2 & i_Other)
	{
		return IntVec2(x - i_Other.x, y - i_Other.y);
	}
};

struct Color
{
	float r, g, b;
	Color() :Color(0.f, 0.f, 0.f) {};
	Color(float i_R, float i_G, float i_B)
		:r(i_R)
		,g(i_G)
		,b(i_B)
	{}
	Color(const Color & i_Other)
		:r(i_Other.r)
		,g(i_Other.g)
		,b(i_Other.b)
	{}
	
	// operators
	Color & operator=(const Color & i_Other)
	{
		r = i_Other.r;
		g = i_Other.g;
		b = i_Other.b;
		return *this;
	}

	// vector conversion
	DirectX::XMFLOAT3 ToVec3() const
	{
		return DirectX::XMFLOAT3(r, g, b);
	}

	DirectX::XMFLOAT4 ToVec4() const
	{
		return DirectX::XMFLOAT4(r, g, b, 1.f);
	}
};

const Color Black(0.f, 0.f, 0.f);
const Color Red(0.f, 0.f, 0.f);
const Color Blue(0.f, 0.f, 0.f);
const Color Green(0.f, 0.f, 0.f);


// Helpers
// return the size of an DXGI_FORMAT element
UINT SizeOfFormatElement(DXGI_FORMAT i_Format);
// get the dxgi format equivalent of a wic format
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID & i_WicFormat);
// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID & i_WicFormat);
// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT & i_DxGIFormat);

enum PopUpIcon
{
	eError		= MB_ICONERROR,
	eWarning	= MB_ICONWARNING,
};

// Debug
#ifdef _DEBUG
void OutputDebug(const char * i_Text, ...);
void PopUpWindow(PopUpIcon i_Icon, const char * i_Notif, const char * i_Text, ...);

#define PRINT_DEBUG(i_Text, ...)	OutputDebug(i_Text, __VA_ARGS__)
#define POPUP_WARNING(i_Text, ...)	PopUpWindow(eWarning, "Warning", i_Text, __VA_ARGS__)
#define POPUP_ERROR(i_Text, ...)	PopUpWindow(eError, "Error", i_Text, __VA_ARGS__)

// assert on DX12 error
#define DX12_ASSERT(i_Hres)				\
do {									\
	if (FAILED(i_Hres))					\
	{									\
		PRINT_DEBUG("", #i_Hres);		\
		POPUP_ERROR("", #i_Hres);		\
		DEBUG_BREAK;					\
	}									\
while(false)


// Assert e
#define ASSERT(i_Condition)												\
do																		\
{																		\
	if (!i_Condition)													\
	{																	\
		PopUpWindow(eError, "Assert", "Assert error file : %s [%i]",	\
				__FILE__, __LINE__);									\
		Assert(i_Text, ...);											\
	}																	\
} while (false)

#else

#define PRINT_DEBUG(i_Text, ...)	
#define PRINT_DEBUG(i_Text, ...)	OutputDebug(i_Text, ...)
#define POPUP_WARNING(i_Text, ...)	PopUpWindow(eWarning, "Warning", i_Text, ...)

#define DX12_ASSERT(i_Hres)		i_Hres
#define ASSERT(i_Condition)		i_Condition

#endif
