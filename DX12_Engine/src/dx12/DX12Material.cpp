#include "dx12/DX12Material.h"

DX12Material::DX12Material(const DX12MaterialDesc & i_Desc)
	:m_ColorAmbient(i_Desc.Ka)
	,m_ColorDiffuse(i_Desc.Kd)
	,m_ColorSpecular(i_Desc.Ks)
	,m_ColorEmissive(i_Desc.Ke)
	,m_HaveChanged(true)
{
	// manage if the material have texture or not
	m_Textures[eDiffuse]	= i_Desc.map_Kd;
	m_Textures[eAmbient]	= i_Desc.map_Ka;
	m_Textures[eSpecular]	= i_Desc.map_Ks;
}

DX12Material::~DX12Material()
{
}

void DX12Material::SetTexture(DX12Texture * i_Texture, ETextureType i_Type)
{
	if (i_Type < eCount)
	{
		m_Textures[i_Type] = i_Texture;
	}
}

bool DX12Material::HaveTexture(ETextureType i_Type) const
{
	if (i_Type < eCount)
	{
		return m_Textures[i_Type] != nullptr;
	}
	return false;
}

void DX12Material::SetAmbientColor(const Color & i_Color)
{
	m_HaveChanged = true;
}

void DX12Material::SetDiffuseColor(const Color & i_Color)
{
	m_HaveChanged = true;

}

void DX12Material::SetEmissiveColor(const Color & i_Color)
{
	m_HaveChanged = true;

}

void DX12Material::SetSpecularColor(const Color & i_Color)
{
	m_HaveChanged = true;

}

void DX12Material::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList)
{
	if (m_HaveChanged)
	{
		// push constant buffer to the gpu

	}


	// bind the constant buffer and texture to the commandlist

}
