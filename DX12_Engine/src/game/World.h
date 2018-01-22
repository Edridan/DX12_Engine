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
	float GetCurrentFrameTime() const;	// get the current frame time (from last draw buffer)

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

	

	std::vector<Actor *>	m_RootActors;
};

// Implementation
template <class _Actor>
_Actor * World::SpawnActor(Actor * i_Parent /* = nullptr */)
{

}