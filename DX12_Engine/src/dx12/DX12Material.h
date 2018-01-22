// material that contains data to render properly objects (color, textures...)

#pragma once

#include "dx12/DX12Utils.h"

#include <string>

// class predef
class DX12Texture;

class DX12Material
{
public:
	// struct
	struct DX12MaterialDesc
	{
		Color Ka, Kd, Ks, Ke;
		float Ns;
		DX12Texture *	map_Kd, * map_Ks, * map_Ke;

		// default constructor
		DX12MaterialDesc()
			:Ka(color::Black)
			,Kd(color::Black)
			,Ks(color::Black)
			,Ke(color::Black)
			,Ns(0.f)
			,map_Kd(nullptr)
			,map_Ks(nullptr)
			,map_Ke(nullptr)
		{}
	};


	DX12Material(const DX12MaterialDesc & i_Desc);
	~DX12Material();

	// material management





private:
	// color of material
	Color	m_ColorAmbient;
	Color	m_ColorDiffuse;
	Color	m_ColorSpecular;
	Color	m_ColorEmissive;

	// textures of the material (if null means not found or not used)
	DX12Texture *	m_TextureDiffuse;
	DX12Texture *	m_TextureSpecular;
	DX12Texture *	m_TextureAmbient;

	// other
	float	m_SpecularExponent;
};