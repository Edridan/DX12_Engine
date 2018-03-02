#include "Actor.h"

// components
#include "components/RenderComponent.h"
// game
#include "engine/World.h"
// engine
#include "engine/Engine.h"
// dx12
#include "dx12/DX12RenderEngine.h"
// resources
#include "resource/ResourceManager.h"
#include "resource/DX12Mesh.h"
#include "resource/Material.h"
#include "resource/Mesh.h"

XMMATRIX Actor::GetWorldTransform()
{
	XMMATRIX thisMat = XMLoadFloat4x4(&m_Transform.GetMatrix());

	if (m_Parent != nullptr)
	{
		thisMat = m_Parent->GetWorldTransform() * thisMat;
	}
	
	return thisMat;
}

bool Actor::IsRoot() const
{
	return (m_Parent == nullptr);
}

bool Actor::HaveChild() const
{
	return (m_Children.size() != 0);
}

bool Actor::IsEnabled() const
{
	return m_Enabled;
}

bool Actor::NeedTick() const
{
	return m_NeedTick && m_Enabled;
}

bool Actor::IsHidden() const
{
	return m_Hidden;
}

bool Actor::NeedRendering() const
{
	return ((m_RenderComponent != nullptr) || (m_LightComponent != nullptr)) && (!m_Hidden);
}

UINT Actor::ChildCount() const
{
	return (UINT)m_Children.size();
}

Actor * Actor::GetChild(UINT i_Index) const
{
	ASSERT(i_Index < m_Children.size());
	return m_Children[i_Index];
}

UINT64 Actor::GetId() const
{
	return m_Id;
}

const std::wstring & Actor::GetName() const
{
	return m_Name;
}

World * Actor::GetWorld() const
{
	return m_World;
}

UINT Actor::GetComponentCount() const
{
	return (UINT)m_Components.size();
}

void Actor::DetachComponent(ActorComponent * i_Component)
{
	if (i_Component == m_LightComponent)		DetachLightComponent();
	else if (i_Component == m_RenderComponent)	DetachRenderComponent();
	else										DetachComponentInternal(i_Component);
}

void Actor::AttachRenderComponent(const RenderComponent::RenderComponentDesc & i_ComponentDesc)
{
	ASSERT(m_RenderComponent == nullptr);
	if (m_RenderComponent != nullptr)		return;

	RenderComponent * component = new RenderComponent(i_ComponentDesc, this);

	if (AttachComponentInternal(component))
	{
		m_RenderComponent = component;
	}
}

bool Actor::DetachRenderComponent()
{
	ASSERT(m_RenderComponent != nullptr);
	if (m_RenderComponent == nullptr)	return false;

	DetachComponentInternal(m_RenderComponent);

	m_RenderComponent = nullptr;

	return true;
}

RenderComponent * Actor::GetRenderComponent() const
{
	return m_RenderComponent;
}

void Actor::AttachLightComponent(const LightComponent::LightDesc & i_Desc)
{
	ASSERT(m_LightComponent == nullptr);
	if (m_LightComponent != nullptr) return;

	LightComponent * component = new LightComponent(i_Desc, this);

	if (AttachComponentInternal(component))
	{
		m_LightComponent = component;
	}
}

bool Actor::DetachLightComponent()
{
	ASSERT(m_LightComponent != nullptr);
	if (m_LightComponent == nullptr)		return false;

	DetachComponentInternal(m_LightComponent);

	m_LightComponent = nullptr;

	return true;
}

LightComponent * Actor::GetLightComponent() const
{
	return m_LightComponent;
}

#ifdef WITH_EDITOR
ActorComponent * Actor::GetComponent(UINT i_Index) const
{
	return m_Components[i_Index];
}
void Actor::SetName(const std::wstring & i_NewName)
{
	m_Name = i_NewName;
}
#endif

Actor::Actor(const ActorDesc & i_Desc, World * i_World)
	:m_Children()
	,m_World(i_World)
	,m_Parent(nullptr)
	,m_Transform()
	,m_Enabled(true)
	,m_Hidden(false)
	,m_NeedTick(false)
	// components
	,m_RenderComponent(nullptr)
	,m_LightComponent(nullptr)
{
	// initialize the object from the desc
	m_NeedTick		= i_Desc.NeedTick;
	m_Name			= i_Desc.Name;

	if (i_Desc.Id == (UINT64)-1)
	{
		// create an id here
		m_Id = (UINT64)this;
	}
	else
	{
		// already specified id
		m_Id = i_Desc.Id;
	}

	// the actor have a mesh component attached to it
	if (i_Desc.Mesh != "")
	{
		std::string filepath, meshName;
		size_t p = i_Desc.Mesh.find(':');

		if (p != std::string::npos)
		{
			meshName = i_Desc.Mesh.substr(p + 1);
			filepath = i_Desc.Mesh.substr(0, p);
		}
		else
		{
			filepath = i_Desc.Mesh;
		}

		// component description
		ResourceManager * manager = Engine::GetInstance().GetResourceManager();
		// Mesh retreiving
		Mesh * mesh = manager->LoadMesh(i_Desc.Mesh.c_str());	// retreive the mesh from resource manager

		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		if (mesh != nullptr)
		{
			// fully load the mesh : create mesh and objects per shapes
			if (meshName == "" && mesh->IsMultiMesh())
			{
				// create child actors
				ActorDesc childDesc;
				childDesc.Mesh = i_Desc.Mesh;

				for (size_t i = 0; i < mesh->GetMeshCount(); ++i)
				{
					// retreive the mesh
					DX12Mesh * meshBuffer = mesh->GetMeshBuffer(i);

					// setup name
					std::wstring wShapeName;
					std::string shapeName = meshBuffer->GetName();
					String::Utf8ToUtf16(wShapeName, shapeName);
					childDesc.Name = m_Name.append(wShapeName);
					childDesc.Mesh = filepath + ":" + shapeName;

					m_World->SpawnActor(childDesc, this);
				}
			}
			else if (meshName == "" && !mesh->IsMultiMesh())
			{
				RenderComponent::RenderComponentDesc componentDesc;
				
				// retreive the material/mesh buffer
				componentDesc.Mesh = mesh->GetMeshBuffer(0);
				componentDesc.Material = mesh->GetMaterial(0, 0);

				// load the main shape
				AttachRenderComponent(componentDesc);

			}
			else if (meshName != "")
			{
				RenderComponent::RenderComponentDesc componentDesc;

				// retreive the material/mesh buffer
				componentDesc.Mesh = mesh->GetMeshBuffer(meshName);
#ifdef ENGINE_DEBUG
				if (mesh->GetMaterialCount(meshName) == 0)
					componentDesc.Material = manager->GetMaterialByName("Default")->GetDX12Material();	// load default
				else
					componentDesc.Material = mesh->GetMaterial(meshName, 0);
#else
				// crash if not debug
				componentDesc.Material = mesh->GetMaterial(meshName, 0);
#endif /* ENGINE_DEBUG */

				// load the main shape
				AttachRenderComponent(componentDesc);
			}
			else
			{
				ASSERT_ERROR("Unknown error");
			}

			//// load root mesh or all submeshes in different actors
			//if (i_Desc.SubMeshId == (UINT)-1)
			//{
			//	componentDesc.Mesh = mesh->GetMeshBuffer();
			//	//mesh->GetTextures(componentDesc.Textures);

			//	// if the current mesh have some submesh
			//	if (mesh->HaveSubMeshes())
			//	{
			//		// load them into other game objects
			//		ActorDesc childDesc;

			//		childDesc.Mesh = i_Desc.Mesh;
			//		childDesc.Name = m_Name.append(L"_SubObject");
			//		
			//		for (size_t i = 0; i < mesh->SubMeshesCount(); ++i)
			//		{
			//			// load submeshes
			//			childDesc.SubMeshId = (UINT)i;
			//			m_World->SpawnActor(childDesc, this);
			//		}
			//	}
			//}
			//else
			//{
			//	componentDesc.Mesh = mesh->GetSubMeshes(i_Desc.SubMeshId);
			//	//mesh->GetTextures(componentDesc.Textures, i_Desc.SubMeshId);
			//}

			//AttachRenderComponent(componentDesc);
		}
		else
		{
			PRINT_DEBUG("Error, unable to get mesh %s", i_Desc.Mesh.c_str());
		}
	}
}

Actor::Actor(World * i_World)
	:m_Children()
	,m_World(i_World)
	,m_Parent(nullptr)
	,m_Transform()
	,m_Enabled(true)
	,m_NeedTick(false)
	// components
	,m_RenderComponent(nullptr)
{
	// empty actors : need to be managed by child class
}

Actor::~Actor()
{
}

void Actor::Tick(float i_Elapsed)
{
	// Basically do nothing (a default actor is not updated)
}

void Actor::Created()
{
	// Basically do nothing
}

void Actor::Destroyed()
{
	// Basically do nothing
}

void Actor::Render()
{
	// To do : 
	// push the render component to the render list
}

bool Actor::AttachComponentInternal(ActorComponent * i_Component)
{
	auto itr = m_Components.begin();

	while (itr != m_Components.end())
	{
		if ((*itr) == i_Component)
		{
			ASSERT_ERROR("Component already added to the actor");
			return false;
		}
		++itr;
	}

	m_Components.push_back(i_Component);
	return true;
}

bool Actor::DetachComponentInternal(ActorComponent * i_Component)
{
	auto itr = m_Components.begin();

	while (itr != m_Components.end())
	{
		if ((*itr) == i_Component)
		{
			m_Components.erase(itr);
			return true;
		}
		++itr;
	}

	ASSERT_ERROR("Component is not in the list");
	return false;
}
