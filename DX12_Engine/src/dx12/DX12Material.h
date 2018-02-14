// material that contains data to render properly objects (color, textures...)

#pragma once

#include "dx12/DX12Utils.h"
#include "dx12/DX12Shader.h"

#include <string>

// class predef
class DX12Texture;
// root and pipeline state
class DX12RootSignature;
class DX12PipelineState;


class DX12Material
{
public:
	// struct
	struct DX12MaterialDesc
	{
		std::string Name;
		Color Ka, Kd, Ks, Ke;
		float Ns;
		DX12Texture *	map_Kd, * map_Ks, * map_Ka;

		// default constructor
		DX12MaterialDesc()
			:Name("Default")
			// set default color to pink (error color)
			,Ka(color::Pink)
			,Kd(color::Pink)
			,Ks(color::Pink)
			,Ke(color::Pink)
			,Ns(1000.f)
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

	void		SetAmbientColor(const Color & i_Color);
	void		SetDiffuseColor(const Color & i_Color);
	void		SetEmissiveColor(const Color & i_Color);
	void		SetSpecularColor(const Color & i_Color);
	// reset the material
	void		Set(const DX12MaterialDesc & i_Desc);

	// flags
	bool		HaveTexture(ETextureType i_Type) const;
	bool		IsCompatibleWithFlags(UINT64 i_ElementFlag) const;

	// id
	UINT64		GetId() const;

	// dx12
	bool		NeedUpdate() const;
	void		UpdateConstantBufferView();
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const;

private:
	// helpers
	void		CreateShaderCode(std::wstring & o_Code, const DX12MaterialDesc & i_Desc, DX12Shader::EShaderType i_Type);
	
	// color of material
	Color	m_ColorAmbient;
	Color	m_ColorDiffuse;
	Color	m_ColorSpecular;
	Color	m_ColorEmissive;

	// name
	std::string	m_Name;

	// textures of the material (if null means not found or not used)
	DX12Texture *			m_Textures[eCount];
	ID3D12DescriptorHeap *	m_Descriptors[eCount];	// descriptor count
	// id for the material
	UINT64					m_Id;
	// pipeline state object
	DX12RootSignature *		m_RootSignature;
	DX12PipelineState *		m_PipelineState;

	// other
	float	m_SpecularExponent;

	// management
	bool		m_HaveChanged;
	ADDRESS_ID	m_ConstantBuffer;
};