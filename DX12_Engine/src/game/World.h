// scene definition
// this contains all actors (3D) and UI logic for rendering

#pragma once

#include <vector>

// class predef
class Actor;

class World
{
public:
	World();
	~World();

	// world public functions can be called by actors
	float GetFrameTime() const;	// get the last elapsed time

	// actor management
	// spawn specific overriden actors
	template <class _Actor>
	_Actor * SpawnActor(Actor * i_Parent = nullptr);

	// spawn default actor
	Actor *	SpawnActor(Actor * i_Parent = nullptr);
	bool	DeleteActor(Actor * i_ActorToRemove, bool i_RemoveChildren = true);

	bool	AttachActor(Actor * i_Parent, Actor * i_Child);
	bool	DetachActor(Actor * i_ActorToDetach);

	friend class Engine;
private:

	// call by engine class (this tick each actor that need a tick)
	void		TickWorld(float i_Elapsed);

	// actors management
	std::vector<Actor *>	m_RootActors;
	std::vector<Actor *>	m_Actors;

	float			m_FrameTime;
};

// Implementation
template <class _Actor>
_Actor * World::SpawnActor(Actor * i_Parent /* = nullptr */)
{
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