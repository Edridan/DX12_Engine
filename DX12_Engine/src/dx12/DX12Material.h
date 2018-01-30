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
		DX12Texture *	map_Kd, * map_Ks, * map_Ka;

		// default constructor
		DX12MaterialDesc()
			:Ka(color::Black)
			,Kd(color::Black)
			,Ks(color::Black)
			,Ke(color::Black)
			,Ns(0.f)
			,map_Kd(nullptr)
			,map_Ks(nullptr)
			,map_Ka(nullptr)
		{}
	};

	// textures type managed by materials
	enum ETextureType
	{
		eAmbient,
		eSpecular,
		eDiffuse,

		eCount,
	};

	DX12Material(const DX12MaterialDesc & i_Desc);
	~DX12Material();

	// material management
	void		SetTexture(DX12Texture * i_Texture, ETextureType i_Type);
	bool		HaveTexture(ETextureType i_Type) const;
	void		SetAmbientColor(const Color & i_Color);
	void		SetDiffuseColor(const Color & i_Color);
	void		SetEmissiveColor(const Color & i_Color);
	void		SetSpecularColor(const Color & i_Color);

	// rendering
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);

private:
	// color of material
	Color	m_ColorAmbient;
	Color	m_ColorDiffuse;
	Color	m_ColorSpecular;
	Color	m_ColorEmissive;

	// textures of the material (if null means not found or not used)
	DX12Texture *	m_Textures[eCount];

	// other
	float	m_SpecularExponent;

	// management
	bool		m_HaveChanged;
	ADDRESS_ID	m_ConstantBuffer;
};