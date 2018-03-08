// lighting data definition

#pragma once

#include <DirectXMath.h>

// define
#define			MAX_LIGHT		64

namespace Light
{
	// defines/enum
	enum ELightType
	{
		ePointLight,
		eCount,
	};

	// structure for each light data
	// point light data
	struct LightData
	{
		DirectX::XMFLOAT4		DiffuseColor;
		float					Range;
	};
};