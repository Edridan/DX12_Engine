// this is a base class for an actor
// erverything that can add to a world (Meshes, Characters, Door, Props) are actors
// you can inherit from this class if you need to add behavior to the actor
// an actor can attach itself components

#pragma once

#include <vector>
#include <string>

#include "engine/Transform.h"

// class predef
class World;	// world of the actor

class Actor
{
public:
	// public
	Transform	m_Transform;

	// information
	bool	IsRoot() const;
	bool	HaveChild() const;

	friend class World;
private:
	// actor creation
	Actor(World * i_World, Actor * i_Parent = nullptr);
	~Actor();

	// virtual function
	virtual void	Tick(float i_Elapsed);

	// parenting system
	std::vector<Actor*>		m_Children;
	Actor *					m_Parent;
	
	// world of the actor
	World * const			m_World;
};