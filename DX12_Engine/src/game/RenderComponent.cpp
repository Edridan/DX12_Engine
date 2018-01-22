#include "RenderComponent.h"

#include "engine/Engine.h"
#include "engine/ResourcesManager.h"

#include "dx12/DX12Mesh.h"
#include "dx12/DX12MeshBuffer.h"
#include "dx12/DX12Texture.h"

RenderComponent::RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor)
	:ActorComponent(i_Actor)
	,m_Mesh(nullptr)
	,m_ConstBuffer(UnavailableAdressId)
	,m_PipelineState(nullptr)
	,m_RootSignature(nullptr)
	,m_RenderPass(RenderPass::eOpaqueGeometry)
{
	// retreive the engine and load the mesh if needed
	ResourcesManager * manager = Engine::GetInstance().GetResourcesManager();
	DX12Mesh * mesh = manager->GetMesh(i_Desc.Mesh.c_str());

	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	if (mesh != nullptr)
	{
		if (i_Desc.SubMeshId == (UINT)-1)
		{
			m_Mesh = mesh->GetRootMesh();
			mesh->GetTextures(m_Textures);
		}
		else
		{
			m_Mesh = mesh->GetSubMeshes(i_Desc.SubMeshId);
			mesh->GetTextures(m_Textures, i_Desc.SubMeshId);
		}

		// retreive the pipeline state object depending the elements flags
		DX12RenderEngine::PipelineStateObject * pso = render.GetPipelineStateObject(m_Mesh->GetElementFlags());

		m_PipelineState = pso->m_PipelineState;
		m_RootSignature = pso->m_DefaultRootSignature;

		// retreive a constant buffer address
		m_ConstBuffer = render.ReserveConstantBufferVirtualAddress();
	}
}

RenderComponent::~RenderComponent()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// cleanup resources
	if (m_Mesh) delete m_Mesh;
	
	if (m_ConstBuffer != UnavailableAdressId)
		render.ReleaseConstantBufferVirtualAddress(m_ConstBuffer);
}

void RenderComponent::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList)
{
	if (m_Mesh != nullptr && m_Actor != nullptr)
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		// add pso and root signature to the commandlist
		i_CommandList->SetGraphicsRootSignature(m_RootSignature);
		// Setup the pipeline state
		i_CommandList->SetPipelineState(m_PipelineState);

		// push const buffer
		if (m_ConstBuffer != UnavailableAdressId)
		{
			i_CommandList->SetGraphicsRootConstantBufferView(0, render.GetConstantBufferUploadVirtualAddress(m_ConstBuffer));
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