// scene definition
// this contains all actors (3D) and UI logic for rendering

#pragma once

#include <vector>

#include "Actor.h"
#include "dx12/DX12Utils.h"

class Camera;
class RenderList;

class World
{
public:
	struct WorldDesc
	{
		UINT		MaxActors = 0;
		Rect		Bounds = { 0.f, 0.f, 0.f, 0.f };	// unlimited bounds by default

		// initial camera position
		XMFLOAT4	CameraPosition = XMFLOAT4(0.f, 0.f, -1.f, 1.f);
		XMFLOAT4	CameraTarget = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		bool		UseCameraProjection = false;
		XMMATRIX	CameraProjection = XMMatrixIdentity();
	};

	World(const WorldDesc & i_WorldDesc);
	~World();

	// world public functions can be called by actors
	float	GetFrameTime() const;	// get the last elapsed time
	UINT	GetActorCount() const;	// all actor count
	UINT	GetRootActorCount() const;

	// actor management
	// spawn specific overriden actors
	template <class _Actor>
	_Actor * SpawnActor(Actor * i_Parent = nullptr);

	// spawn default actor
	Actor *	SpawnActor(const Actor::ActorDesc & i_Desc, Actor * i_Parent = nullptr);
	Actor *	SpawnActor(const Actor::ActorDesc & i_Desc, const Transform & i_Transform, Actor * i_Parent = nullptr);	// Warning : the transform is relative to the parent
	bool	DeleteActor(Actor * i_ActorToRemove, bool i_RemoveChildren = true);

	bool	AttachActor(Actor * i_Parent, Actor * i_Child);
	bool	DetachActor(Actor * i_ActorToDetach);

	Camera *	GetCurrentCamera() const;

	// actor request
	Actor *		GetActorById(UINT64 i_Id) const;
	Actor *		GetRootActorById(UINT64 i_Id) const;
	UINT		GetActorsByName(std::vector<Actor*> o_Array, std::wstring i_Name) const;
	UINT		GetRootActorsByName(std::vector<Actor *> o_Array, std::wstring i_Name) const;
	Actor *		GetRootActorByIndex(size_t i_Index) const;
	Actor *		GetActorByIndex(size_t i_Index) const;


	// clear world
	void		Clear();

	friend class Engine;

private:

	// call by engine class (this tick each actor that need a tick)
	void		TickWorld(float i_Elapsed);
	void		RenderWorld(RenderList * i_RenderList) const;

	// internal call
	void		RenderActor(const Actor * i_Actor, RenderList  * i_RenderList) const;

	// actors management
	std::vector<Actor *>	m_RootActors;
	std::vector<Actor *>	m_Actors;

	// camera management
	Camera *		m_CurrentCamera;	// To do : manage camera

	bool			m_LimitedActorCount;
	float			m_FrameTime;
};

// Implementation
template <class _Actor>
_Actor * World::SpawnActor(Actor * i_Parent /* = nullptr */)
{
	if (m_LimitedActorCount && (m_Actors.size() == m_Actors.capacity()))
	{
		// we alerady have the max actors
		PRINT_DEBUG("Actors count have reached the limit");
		return nullptr;
	}

	// create the actor
	Actor * newActor = new _Actor(this);
	m_Actors.push_back(newActor);

	// add actor to the parent if needed
	if (i_Parent = nullptr)
	{
		newActor->m_Parent = i_Parent;
		i_Parent->m_Children.push_back(newActor);
	}

	return reinterpret_cast<_Actor*>newActor;
}