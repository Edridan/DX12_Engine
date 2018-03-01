#include "RenderComponent.h"

#include "engine/Engine.h"
#include "dx12/DX12RenderEngine.h"
#include "resource/ResourceManager.h"
#include "resource/DX12Mesh.h"
#include "resource/Mesh.h"

RenderComponent::RenderComponent(const RenderComponentDesc & i_Desc, Actor * i_Actor)
	:ActorComponent(i_Actor, "Render Component")
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
		TO_DO;
	}
	else
	{
		// retreive the material default from mesh
		m_Material = i_Desc.Material;
	}


	// assert for debug
	ASSERT(m_Material != nullptr);
	ASSERT(m_Mesh != nullptr);

	// manage constant buffer address
	m_ConstBuffer = render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReserveVirtualAddress();
	//m_Material->UpdateConstantBuffer();
}

RenderComponent::RenderComponent(Actor * i_Actor)
	:ActorComponent(i_Actor, "Render Component")
	,m_Mesh(nullptr)
	,m_ConstBuffer(UnavailableAdressId)
	,m_Material(nullptr)
	,m_RenderPass(RenderPass::eOpaqueGeometry)
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	m_ConstBuffer = render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReserveVirtualAddress();
}

RenderComponent::~RenderComponent()
{
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();
	
	if (m_ConstBuffer != UnavailableAdressId)
		render.GetConstantBuffer(DX12RenderEngine::eTransform)->ReleaseVirtualAddress(m_ConstBuffer);
}

void RenderComponent::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList) const
{
	if (m_Mesh != nullptr && m_Actor != nullptr && m_Material != nullptr &&
		m_Mesh->IsValid() && m_Material->IsValid())
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		//// add pso and root signature to the commandlist
		m_Material->PushPipelineState(i_CommandList);

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

void RenderComponent::SetMaterial(const DX12Material * i_Material)
{
	m_Material = i_Material;
}

const DX12Material * RenderComponent::GetMaterial() const
{
	return m_Material;
}

void RenderComponent::SetMeshBuffer(const DX12Mesh * i_Mesh)
{
	m_Mesh = i_Mesh;
}

const DX12Mesh * RenderComponent::GetMeshBuffer()
{
	return m_Mesh;
}

bool RenderComponent::IsRenderable() const
{
	return (m_Mesh != nullptr && m_Material != nullptr);
}

///////////////////////////////////////
// editor only
#ifdef WITH_EDITOR
#include "ui/UI.h"
#include "engine/Engine.h"

void RenderComponent::DrawUIComponentInternal()
{
	DrawUIMaterial();
	DrawUIMesh();
}

FORCEINLINE void RenderComponent::DrawUIMaterial()
{
	if (ImGui::TreeNode("Material"))
	{
		// retreive materials
		std::vector<std::string> files;
		static const char * items[128];
		static int selectedItem = -1;


		if (m_Material != nullptr)
		{
			selectedItem = 0;
			files.push_back(m_Material->GetName());
		}

		const int currentItem = selectedItem;

		Files::GetFilesInFolder(files, "resources/obj", ".mtl", true);

		for (size_t i = 0; (i < files.size() && i < 128); ++i)
		{
			items[i] = files[i].c_str();
		}

		ImGui::Combo("", &selectedItem, items, Math::Min((int)files.size(), 128));

		if (selectedItem != currentItem)
		{
			// To do : load and change the material
			
		}


		ImGui::TreePop();
	}
}

FORCEINLINE void RenderComponent::DrawUIMesh()
{
	if (ImGui::TreeNode("Mesh"))
	{
		// retreive materials
		std::vector<std::string> files;
		static const char * items[128];
		static int selectedItem = -1;


		if (m_Mesh != nullptr)
		{
			selectedItem = 0;
			files.push_back(m_Mesh->GetName());
		}

		const int currentItem = selectedItem;


		files.push_back("Primitive:Triangle");
		files.push_back("Primitive:Plane");
		files.push_back("Primitive:Cube");

		Files::GetFilesInFolder(files, "resources/obj", ".obj", true);

		for (size_t i = 0; (i < files.size() && i < 128); ++i)
		{
			items[i] = files[i].c_str();
		}

		ImGui::Combo("", &selectedItem, items, Math::Min((int)files.size(), 128));

		if (selectedItem != currentItem)
		{
			std::string fileToLoad = files[selectedItem];
			// change the mesh
			if (String::StartWith(files[selectedItem], "Primitive:"))
			{
				fileToLoad = fileToLoad.substr(10);
			}

			
		}


		ImGui::TreePop();
	}
}
#endif 
