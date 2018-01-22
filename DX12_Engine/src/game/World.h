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


	friend class Engine;
private:

	// call by engine class
	void		TickWorld(float i_Elapsed);

	std::vector<Actor *>	m_RootActors;
};