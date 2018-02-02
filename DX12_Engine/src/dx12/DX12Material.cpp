#include "dx12/DX12Material.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12ConstantBuffer.h"
#include "dx12/DX12PipelineState.h"
#include "dx12/DX12Texture.h"

DX12Material::DX12Material(const DX12MaterialDesc & i_Desc)
	:m_ColorAmbient(i_Desc.Ka)
	,m_ColorDiffuse(i_Desc.Kd)
	,m_ColorSpecular(i_Desc.Ks)
	,m_ColorEmissive(i_Desc.Ke)
	,m_SpecularExponent(i_Desc.Ns)
	,m_HaveChanged(true)
	,m_ConstantBuffer(UnavailableAdressId)
	,m_Name(i_Desc.Name)
	,m_Id((UINT64)this)
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// manage if the material have texture or not
	SetTexture(i_Desc.map_Kd, eDiffuse);
	SetTexture(i_Desc.map_Ks, eSpecular);
	SetTexture(i_Desc.map_Ka, eAmbient);

	// reserve address for constant buffer
	m_ConstantBuffer = render.GetConstantBuffer(DX12RenderEngine::eMaterial)->ReserveVirtualAddress();
	UpdateConstantBufferView();	// update to constant buffer
}

DX12Material::~DX12Material()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	if (m_ConstantBuffer != UnavailableAdressId)
	{
		render.GetConstantBuffer(DX12RenderEngine::eMaterial)->ReleaseVirtualAddress(m_ConstantBuffer);
	}
}

inline void DX12Material::SetTexture(DX12Texture * i_Texture, ETextureType i_Type)
{
	if (i_Type < eCount)
	{
		m_Textures[i_Type] = i_Texture;

		if (i_Texture)
		{
			m_Descriptors[i_Type] = i_Texture->GetDescriptorHeap();
		}
		else
		{
			m_Descriptors[i_Type] = nullptr;
		}
	}
}

void DX12Material::SetAmbientColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorAmbient = i_Color;
}

void DX12Material::SetDiffuseColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorDiffuse = i_Color;
}

void DX12Material::SetEmissiveColor(const Color & i_Color)
{
	m_HaveChanged = true;
	m_ColorEmissive = i_Color;
}

void DX12Material::SetSpecularColor(const Color & i_Color)
{
	m_HaveChanged = true;

}

void DX12Material::Set(const DX12MaterialDesc & i_Desc)
{
	m_ColorAmbient = i_Desc.Ka;
	m_ColorDiffuse = i_Desc.Kd;
	m_ColorSpecular = i_Desc.Ks;
	m_ColorEmissive = i_Desc.Ke;

	m_HaveChanged = true;

	m_ConstantBuffer = UnavailableAdressId;

	m_Name = i_Desc.Name;

	// manage if the material have texture or not
	SetTexture(i_Desc.map_Kd, eDiffuse);
	SetTexture(i_Desc.map_Ks, eSpecular);
	SetTexture(i_Desc.map_Ka, eAmbient);

	m_HaveChanged = true;
}

UINT64 DX12Material::GetId() const
{
	return m_Id;
}

inline bool DX12Material::HaveTexture(ETextureType i_Type) const
{
	if (i_Type < eCount)
	{
		return m_Textures[i_Type] != nullptr;
	}
	return false;
}

bool DX12Material::IsCompatibleWithFlags(UINT64 i_ElementFlag) const
{
	const bool haveTex = HaveTexture(eAmbient) || HaveTexture(eDiffuse) || HaveTexture(eSpecular);

	if (haveTex && !(i_ElementFlag | DX12PipelineState::EElementFlags::eHaveTexcoord))
		return false;

	return true;
}

bool DX12Material::NeedUpdate() const
{
	return false;
}

inline void DX12Material::UpdateConstantBufferView()
{
	struct MaterialStruct
	{
		DirectX::XMFLOAT4		Ka, Kd, Ks, Ke;
		BOOL					Map_A, Map_D, Map_S;
		float					Ns;
	};

	// error management
	if (m_ConstantBuffer == UnavailableAdressId)
	{
		PRINT_DEBUG("Error, the constant buffer address is not set");
		DEBUG_BREAK;
		return;
	}

	if (m_HaveChanged)
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		// push constant buffer to the gpu
		MaterialStruct mat;

		mat.Ka = ColorToVec4(m_ColorAmbient);
		mat.Kd = ColorToVec4(m_ColorDiffuse);
		mat.Ks = ColorToVec4(m_ColorSpecular);
		mat.Ke = ColorToVec4(m_ColorEmissive);

		mat.Map_A = HaveTexture(eAmbient);
		mat.Map_D = HaveTexture(eDiffuse);
		mat.Map_S = HaveTexture(eSpecular);

		mat.Ns = m_SpecularExponent;

		// update the buffer
		render.GetConstantBuffer(DX12RenderEngine::eMaterial)->UpdateConstantBufferForEachFrame(m_ConstantBuffer, &mat, sizeof(MaterialStruct));

		m_HaveChanged = false;
	}
}

void DX12Material::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// error management
	if (m_ConstantBuffer == UnavailableAdressId)
	{
		PRINT_DEBUG("Error, the constant buffer address is not set");
		DEBUG_BREAK;
		return;
	}

	// set descriptors

	// parameter 0 is already used by the CBV for transform so we start to the 
	i_CommandList->SetGraphicsRootConstantBufferView(2, render.GetConstantBuffer(DX12RenderEngine::eMaterial)->GetUploadVirtualAddress(m_ConstantBuffer));

	// bind textures
	for (UINT i = 0; i < ETextureType::eCount; ++i)
	{
		ID3D12DescriptorHeap ** descriptors = (ID3D12DescriptorHeap **)(&m_Descriptors[i]);
		if (HaveTexture((ETextureType)i))
		{
			// update the descriptor for the resources
			i_CommandList->SetDescriptorHeaps(1, descriptors);
			i_CommandList->SetGraphicsRootDescriptorTable(3 + i, m_Textures[i]->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		}
	}

}
