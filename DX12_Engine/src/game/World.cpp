#include "World.h"

#include "game/Actor.h"
#include "dx12/DX12Utils.h"
#include "game/Camera.h"
#include "engine/Engine.h"
#include "engine/RenderList.h"
#include "engine/Debug.h"

World::World(const WorldDesc & i_WorldDesc)
	:m_CurrentCamera(new Camera)
	,m_LimitedActorCount(false)
{
	if (i_WorldDesc.MaxActors != 0)
	{
		m_LimitedActorCount = true;
		m_Actors.reserve(i_WorldDesc.MaxActors);
	}

	// initialize camera
	m_CurrentCamera->m_Position		= i_WorldDesc.CameraPosition;
	m_CurrentCamera->m_Target		= i_WorldDesc.CameraTarget;
	m_CurrentCamera->m_Up			= Vec4Up;

	if (i_WorldDesc.UseCameraProjection)
	{
		m_CurrentCamera->SetProjectionMatrix(i_WorldDesc.CameraProjection);
	}

#ifdef _DEBUG
	// enable freecam in debug
	m_CurrentCamera->SetFreecamEnabled(true);
#endif
}

World::~World()
{
}

float World::GetFrameTime() const
{
	return m_FrameTime;
}

UINT World::GetActorCount() const
{
	return (UINT)m_Actors.size();
}


Actor * World::SpawnActor(const Actor::ActorDesc & i_Desc, Actor * i_Parent)
{
	if (m_LimitedActorCount && (m_Actors.size() == m_Actors.capacity()))
	{
		// we alerady have the max actors
		PRINT_DEBUG("Actors count have reached the limit");
		return nullptr;
	}

	Actor * newActor = new Actor(i_Desc, this);

	// debug print
	{
		XMFLOAT3 pos = newActor->m_Transform.GetPosition();
		PRINT_DEBUG("Spawn actor \"%S\"[id:%i] at location (%.3f, %.3f, %.3f)", 
			newActor->GetName().c_str(), newActor->GetId(), 
			pos.x, pos.y, pos.z);
	}

	m_Actors.push_back(newActor);

	// add actor to the parent if needed
	if (i_Parent = nullptr)
	{
		newActor->m_Parent = i_Parent;
		i_Parent->m_Children.push_back(newActor);
	}

	// push actor on roots actors if needed
	if (newActor->IsRoot())
	{
		m_RootActors.push_back(newActor);
	}

	// call created event
	newActor->Created();

	return newActor;
}

bool World::DeleteActor(Actor * i_ActorToRemove, bool i_RemoveChildren)
{
	auto itr = std::find(m_Actors.begin(), m_Actors.end(), i_ActorToRemove);

	// the actor is not in the list
	if (itr == m_Actors.end())
	{
		return false;
	}

	// erase the actor from the actors list
	m_Actors.erase(itr);

	// remove the actor from the root
	if (i_ActorToRemove->IsRoot())
	{
		auto itr = std::find(m_RootActors.begin(), m_RootActors.end(), i_ActorToRemove);

		// the actor is supposed to be in the root actors
		if (itr == m_RootActors.end())
		{
			DEBUG_BREAK;
			return false;
		}

		m_RootActors.erase(itr);
	}

	// manage children
	if (i_ActorToRemove->HaveChild())
	{
		auto itr = i_ActorToRemove->m_Children.begin();

		while (itr != i_ActorToRemove->m_Children.end())
		{
			if (i_RemoveChildren)
			{
				DeleteActor((*itr), i_RemoveChildren);
			}
			else
			{
				// the actor is now root
				(*itr)->m_Parent = nullptr;
				m_RootActors.push_back(*itr);
			}

			++itr;
		}
	}

	// call event before deletion
	i_ActorToRemove->Destroyed();

	// To do : call to a Destroy event on Actor
	// remove the actor from the memory
	delete i_ActorToRemove;

	return true;
}

bool World::AttachActor(Actor * i_Parent, Actor * i_Child)
{
	if (i_Parent == nullptr || i_Child == nullptr)
		return false;

	// we need to remove the actor from the childs
	if (i_Child->IsRoot())
	{
		auto itr = std::find(m_RootActors.begin(), m_RootActors.end(), i_Child);

		// the actor is supposed to be in the root actors
		if (itr == m_RootActors.end())
		{
			DEBUG_BREAK;
			return false;
		}

		m_RootActors.erase(itr);
	}
	else
	{
		Actor * parent = i_Child->m_Parent;

		// remove the child from the current parent children
		auto itr = std::find(parent->m_Children.begin(), parent->m_Children.end(), i_Child);

		if (itr == parent->m_Children.end())
		{
			DEBUG_BREAK;
			return false;
		}

		parent->m_Children.erase(itr);
	}

	// update parent/childrens
	i_Parent->m_Children.push_back(i_Child);
	i_Child->m_Parent = i_Parent;

	return true;
}

bool World::DetachActor(Actor * i_ActorToDetach)
{
	// if the actor is attached to something
	if (!i_ActorToDetach->IsRoot())
	{
		Actor * parent = i_ActorToDetach->m_Parent;

		// remove the child from the current parent children
		auto itr = std::find(parent->m_Children.begin(), parent->m_Children.end(), i_ActorToDetach);

		if (itr == parent->m_Children.end())
		{
			DEBUG_BREAK;
			return false;
		}

		parent->m_Children.erase(itr);
	}

	// push the actor to the root actors
	m_RootActors.push_back(i_ActorToDetach);

	return true;
}

Camera * World::GetCurrentCamera() const
{
	return m_CurrentCamera;
}

Actor * World::GetActorById(UINT64 i_Id) const
{
	auto itr = m_Actors.begin();
	while (itr != m_Actors.end())
	{
		if ((*itr)->GetId() == i_Id)
		{
			return (*itr);
		}
		++itr;
	}
	// actor not founded
	return nullptr;
}

Actor * World::GetRootActorById(UINT64 i_Id) const
{
	auto itr = m_RootActors.begin();
	while (itr != m_RootActors.end())
	{
		if ((*itr)->GetId() == i_Id)
		{
			return (*itr);
		}
		++itr;
	}
	// actor not founded
	return nullptr;
}

UINT World::GetActorsByName(std::vector<Actor*> o_Array, std::wstring i_Name) const
{
	auto itr = m_Actors.begin();
	UINT actorsFounded = 0;

	while (itr != m_Actors.end())
	{
		if ((*itr)->GetName() == i_Name)
		{
			o_Array.push_back((*itr));
			++actorsFounded;
		}
		++itr;
	}
	// return number of actors founded in the world
	return actorsFounded;
}

UINT World::GetRootActorsByName(std::vector<Actor*> o_Array, std::wstring i_Name) const
{
	auto itr = m_RootActors.begin();
	UINT actorsFounded = 0;

	while (itr != m_RootActors.end())
	{
		if ((*itr)->GetName() == i_Name)
		{
			o_Array.push_back((*itr));
			++actorsFounded;
		}
		++itr;
	}
	// return number of actors founded in the world
	return actorsFounded;
}

void World::Clear()
{
	// clear all game objects
	auto itr = m_Actors.begin();

	while (itr != m_Actors.end())
	{
		// delete actors
		(*itr)->Destroyed();
		delete (*itr);

		++itr;
	}

	// clear and reset vectors
	m_RootActors.clear();
	m_Actors.clear();
}

void World::TickWorld(float i_Elapsed)
{
	// save elapsed time
	m_FrameTime = i_Elapsed;

	// update camera
	m_CurrentCamera->Update(i_Elapsed);	

	// update components
	for (size_t i = 0; i < m_Actors.size(); ++i)
	{
		Actor * actor = m_Actors[i];

		if (actor->NeedTick())
		{
			// first we need to update the actor
			if (actor->NeedTick())
			{
				actor->Tick(m_FrameTime);
			}
		}
	}
}

void World::RenderWorld(RenderList * i_RenderList) const
{
	// take all root actors
	for (size_t i = 0; i < m_RootActors.size(); ++i)
	{
		RenderActor(m_RootActors[i], i_RenderList);
	}
}

void World::RenderActor(const Actor * i_Actor, RenderList * i_RenderList) const
{
	// if the actor is hidden, we do not render his other children
	if (i_Actor->IsHidden())
	{
		return;
	}

	// render the actor
	if (i_Actor->NeedRendering())
	{
		// then we need maybe to render the actor,
		// this mean we push the render component to the render list
		// To do : push the render component to the specific list
		i_RenderList->PushRenderComponent(i_Actor->GetRenderComponent());
	}

	// render if needed the childs
	const std::vector<Actor *> * const children = &(i_Actor->m_Children);

	for (size_t i = 0; i < children->size(); ++i)
	{
		// render actor if needed
		RenderActor((*children)[i], i_RenderList);
	}
}
