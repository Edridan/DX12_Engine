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
	,m_ConstBuffer(UnavailableAdressId)
	,m_Material(nullptr)
	,m_RenderPass(RenderPass::eOpaqueGeometry)
{
	// retreive the engine and load the mesh if needed
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// material management
	if (i_Desc.Material == nullptr)
	{
		// retreive the material default from mesh
		m_Material = new DX12Material(m_Mesh->GetDefaultMaterialDesc());
	}
	else
	{
		if (!m_Mesh->IsCompatible(*i_Desc.Material)) 
		{
			m_Material = new DX12Material(*i_Desc.Material);
		}
		else
		{
			PRINT_DEBUG("Error, the material is not compatible with the mesh");
			return;
		}
	}

	// assert for debug
	ASSERT(m_Material != nullptr);
	ASSERT(m_Mesh != nullptr);

	// retreive a constant buffer address
	//m_ConstBuffer = render.ReserveConstantBufferVirtualAddress();
	m_ConstBuffer = render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReserveVirtualAddress();

	m_Material->UpdateConstantBufferView();
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

		//// add pso and root signature to the commandlist
		//i_CommandList->SetGraphicsRootSignature(m_RootSignature);
		//// Setup the pipeline state
		//i_CommandList->SetPipelineState(m_PipelineState);
		m_Material->SetupPipeline(i_CommandList);

		// push transform buffer
		if (m_ConstBuffer != UnavailableAdressId)
		{
			i_CommandList->SetGraphicsRootConstantBufferView(0, render.GetConstantBuffer(DX12RenderEngine::eTransform)->GetUploadVirtualAddress(m_ConstBuffer));
		}

		// push the global buffer
		i_CommandList->SetGraphicsRootConstantBufferView(1,	// 1 for b1 see the dx12 render engine constant buffer placement 
			render.GetConstantBuffer(DX12RenderEngine::eGlobal)->GetUploadVirtualAddress(0U));

		m_Material->PushOnCommandList(i_CommandList);

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

void RenderComponent::SetMaterial(const DX12Material::DX12MaterialDesc & i_Desc)
{
	if (m_Mesh->IsCompatible(i_Desc))
	{
		m_Material->Set(i_Desc);
		m_Material->UpdateConstantBufferView();
	}
	else
	{
		PRINT_DEBUG("[Warning] setting a non compatible material to mesh %s", m_Mesh->GetName().c_str());
	}
}

DX12Material * RenderComponent::GetMaterial()
{
	return m_Material;
}