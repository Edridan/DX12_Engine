#include "Actor.h"

// components
#include "game/RenderComponent.h"
// game
#include "game/World.h"
// engine
#include "engine/Engine.h"
#include "engine/ResourcesManager.h"
// dx12
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12Mesh.h"

// static definition
UINT64 Actor::s_ActorInstanced = 0;

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
	return (m_RenderComponent != nullptr) && (!m_Hidden);
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

void Actor::AttachRenderComponent(const RenderComponent::RenderComponentDesc & i_ComponentDesc)
{
	m_RenderComponent = new RenderComponent(i_ComponentDesc, this);
}

bool Actor::DetachRenderComponent()
{
	return false;
}

RenderComponent * Actor::GetRenderComponent() const
{
	return m_RenderComponent;
}

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
{
	// initialize the object from the desc
	m_NeedTick		= i_Desc.NeedTick;
	m_Name			= i_Desc.Name;

	if (i_Desc.Id == (UINT64)-1)
	{
		// create an id here
		m_Id = s_ActorInstanced++;
	}
	else
	{
		// already specified id
		m_Id = i_Desc.Id;
	}

	// the actor have a mesh component attached to it
	if (i_Desc.Mesh != L"")
	{
		// component description
		RenderComponent::RenderComponentDesc componentDesc;
		// load the mesh
		ResourcesManager * manager = Engine::GetInstance().GetResourcesManager();
		DX12Mesh * mesh = manager->GetMesh(i_Desc.Mesh.c_str());	// load mesh if needed

		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		if (mesh != nullptr)
		{
			// load root mesh or all submeshes in different actors
			if (i_Desc.SubMeshId == (UINT)-1)
			{
				componentDesc.Mesh = mesh->GetRootMesh();
				//mesh->GetTextures(componentDesc.Textures);

				// if the current mesh have some submesh
				if (mesh->HaveSubMeshes())
				{
					// load them into other game objects
					ActorDesc childDesc;

					childDesc.Mesh = i_Desc.Mesh;
					childDesc.Name = m_Name.append(L"_SubObject");
					
					for (size_t i = 0; i < mesh->SubMeshesCount(); ++i)
					{
						// load submeshes
						childDesc.SubMeshId = (UINT)i;
						m_World->SpawnActor(childDesc, this);
					}
				}
			}
			else
			{
				componentDesc.Mesh = mesh->GetSubMeshes(i_Desc.SubMeshId);
				//mesh->GetTextures(componentDesc.Textures, i_Desc.SubMeshId);
			}

			m_RenderComponent = new RenderComponent(componentDesc, this);
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
