#include "World.h"

World::World()
{
}

World::~World()
{
}

float World::GetFrameTime() const
{
	return 0.0f;
}

float World::GetCurrentFrameTime() const
{
	return 0.0f;
}

Actor * World::SpawnActor(Actor * i_Parent)
{
	return nullptr;
}

bool World::DeleteActor(Actor * i_ActorToRemove, bool i_RemoveChildren)
{
	return false;
}

bool World::AttachActor(Actor * i_Parent, Actor * i_Child)
{
	return false;
}

bool World::DetachActor(Actor * i_ActorToDetach)
{
	return false;
}

void World::TickWorld(float i_Elapsed)
{
}
