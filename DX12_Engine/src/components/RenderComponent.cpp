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
	if (i_Desc.Material != nullptr)
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
#include "resource/ResourceManager.h"
#include "resource/Material.h"
#include "resource/Mesh.h"
#include "resource/DX12Mesh.h"

void RenderComponent::DrawUIComponentInternal()
{
	DrawUIMaterial();
	DrawUIMesh();
}

FORCEINLINE void RenderComponent::DrawUIMaterial()
{
	if (ImGui::TreeNode("Material"))
	{
		ResourceManager * manager = Engine::GetInstance().GetResourceManager();

		static Actor * actor = nullptr;
		std::vector<std::string> files;
		std::vector<std::string> materials;
		static Material * mat = nullptr;
		static const char * charFiles[128];
		static const char * charMat[128];
		static int selectedMat = -1;
		static int selectedFile = -1;

		files.clear();
		materials.clear();

		// initialize if needed
		if (actor != m_Actor)
		{
			actor = m_Actor;
			selectedFile = -1;
			selectedMat = -1;
		}

		if (mat != nullptr)
		{
			selectedFile = 0;
			files.push_back(mat->GetFilepath());
		}

		const int currentFile = selectedFile;
		int currentMat = selectedMat;

		files.push_back("Default");

		Files::GetFilesInFolder(files, "resources/obj", ".mtl", true);

		for (size_t i = 0; (i < files.size() && i < 128); ++i)
		{
			charFiles[i] = files[i].c_str();
		}

		ImGui::Combo("File", &selectedFile, charFiles, Math::Min((int)files.size(), 128));

		if (selectedFile != currentFile)
		{
			std::string fileToLoad = files[selectedFile];
			Material * newMat = manager->LoadMaterial(fileToLoad);
			ASSERT(newMat != nullptr);

			if (mat != newMat)
			{
				currentMat = selectedMat = -1;
				mat = newMat;
			}
		}

		if (mat)
		{
			for (size_t i = 0; i < mat->GetMaterialCount() ;++i)
			{
				materials.push_back(mat->GetDX12Material(i)->GetName());
				charMat[i] = materials[i].c_str();
			}
		}
		else if (m_Material != nullptr)
		{
			selectedMat = 0;
			materials.push_back(m_Mesh->GetName());
			charMat[0] = materials[0].c_str();
		}

		ImGui::Combo("Shape", &selectedMat, charMat, Math::Min((int)materials.size(), 128));

		if (selectedMat != currentMat && mat)
		{
			// change the shape
			m_Material = mat->GetDX12Material(selectedMat);
		}

		ImGui::TreePop();
	}
}

FORCEINLINE void RenderComponent::DrawUIMesh()
{
	if (ImGui::TreeNode("Mesh"))
	{
		ResourceManager * manager = Engine::GetInstance().GetResourceManager();

		// retreive materials
		static const char * charFiles[128];
		static const char * charShapes[128];
		static int selectedMesh = -1;
		static int selectedShape = -1;
		static Actor * actor = nullptr;
		static Mesh * mesh = nullptr;
		std::vector<std::string> files;
		std::vector<std::string> shapes;

		files.clear();
		shapes.clear();

		// initialize if needed
		if (actor != m_Actor)
		{
			actor = m_Actor;
			selectedMesh = -1;
			selectedShape = -1;
		}

		if (mesh != nullptr)
		{
			selectedMesh = 0;
			files.push_back(mesh->GetFilepath());
		}

		const int currentMesh = selectedMesh;
		int currentShape = selectedShape;

		files.push_back("Primitive:Triangle");
		files.push_back("Primitive:Plane");
		files.push_back("Primitive:Cube");

		Files::GetFilesInFolder(files, "resources/obj", ".obj", true);

		for (size_t i = 0; (i < files.size() && i < 128); ++i)
		{
			charFiles[i] = files[i].c_str();
		}

		ImGui::Combo("Mesh", &selectedMesh, charFiles, Math::Min((int)files.size(), 128));

		if (selectedMesh != currentMesh)
		{
			std::string fileToLoad = files[selectedMesh];
			Mesh * newMesh = manager->LoadMesh(fileToLoad);
			ASSERT(newMesh != nullptr);

			if (mesh != newMesh)
			{
				currentShape = selectedShape = -1;
				mesh = newMesh;
			}
		}

		if (mesh)
		{
			for (size_t i = 0; i < mesh->GetMeshCount(); ++i)
			{
				shapes.push_back(mesh->GetMeshBuffer(i)->GetName());
				charShapes[i] = shapes[i].c_str();
			}
		}
		else if (m_Mesh != nullptr)
		{
			selectedShape = 0;
			shapes.push_back(m_Mesh->GetName());
			charShapes[0] = shapes[0].c_str();
		}

		ImGui::Combo("Shape", &selectedShape, charShapes, Math::Min((int)shapes.size(), 128));

		if (selectedShape != currentShape && mesh)
		{
			// change the shape
			m_Mesh = mesh->GetMeshBuffer(selectedShape);
		}

		ImGui::TreePop();
	}
}
#endif 
