#include "RenderComponent.h"

#include "engine/Engine.h"
#include "engine/ResourcesManager.h"

#include "dx12/DX12Mesh.h"
#include "dx12/DX12MeshBuffer.h"
#include "dx12/DX12Texture.h"
#include "dx12/DX12RenderEngine.h"

RenderComponent::RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor)
	:ActorComponent(i_Actor)
	,m_Mesh(i_Desc.Mesh)
	,m_Textures(i_Desc.Textures)
	,m_ConstBuffer(UnavailableAdressId)
	,m_PipelineState(nullptr)
	,m_RootSignature(nullptr)
	,m_Material(nullptr)
	,m_RenderPass(RenderPass::eOpaqueGeometry)
{
	// retreive the engine and load the mesh if needed
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// retreive the pipeline state object depending the elements flags
	DX12RenderEngine::PipelineStateObject * pso = render.GetPipelineStateObject(m_Mesh->GetElementFlags());

	if (pso)
	{
		m_PipelineState = pso->m_PipelineState;
		m_RootSignature = pso->m_DefaultRootSignature;
	}
	else
	{
		PRINT_DEBUG("Error when creating mesh : PSO do not exist for this type of vertex declaration");
		POPUP_ERROR("Error when creating mesh : PSO do not exist for this type of vertex declaration");
		DEBUG_BREAK;
	}

	// material management
	//if (i_Desc.Material == nullptr)
	//{
	//	// retreive the material default from mesh
	//}
	//else
	//{
	//	if (!m_Mesh->IsCompatible(*i_Desc.Material)) 
	//	{
	//		m_Material = new DX12Material(*i_Desc.Material);
	//	}
	//	else
	//	{
	//		PRINT_DEBUG("Error, the material is not compatible with the mesh");
	//		return;
	//	}
	//}

	// retreive a constant buffer address
	//m_ConstBuffer = render.ReserveConstantBufferVirtualAddress();
	m_ConstBuffer = render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReserveVirtualAddress();
}

RenderComponent::~RenderComponent()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// cleanup resources
	if (m_Mesh) delete m_Mesh;
	
	if (m_ConstBuffer != UnavailableAdressId)
		render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReleaseVirtualAddress(m_ConstBuffer);
}

void RenderComponent::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	if (m_Mesh != nullptr && m_Actor != nullptr /*&& m_Material != nullptr*/)
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		// add pso and root signature to the commandlist
		i_CommandList->SetGraphicsRootSignature(m_RootSignature);
		// Setup the pipeline state
		i_CommandList->SetPipelineState(m_PipelineState);

		// push const buffer
		if (m_ConstBuffer != UnavailableAdressId)
		{
			i_CommandList->SetGraphicsRootConstantBufferView(0, render.GetConstantBuffer(DX12RenderEngine::eTransform)->GetUploadVirtualAddress(m_ConstBuffer));
		}

		// push needed textures
		for (size_t i = 0; i < m_Textures.size(); ++i)
		{
			m_Textures[i]->PushOnCommandList(i_CommandList);
		}

		// push the mesh on the commandlist (setup vertices)
		m_Mesh->PushOnCommandList(i_CommandList);
	}
}

RenderComponent::RenderPass RenderComponent::GetRenderPass() const
{
	return m_RenderPass;
}

UINT64 RenderComponent::GetRenderFlags() const
{
	return m_RenderFlags;
}

ADDRESS_ID RenderComponent::GetConstBufferAddress() const
{
	return m_ConstBuffer;
}

void RenderComponent::SetTexture(DX12Texture * i_Texture)
{
	m_Textures.clear();
	m_Textures.push_back(i_Texture);
}