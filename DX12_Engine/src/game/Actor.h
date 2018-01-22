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
class ActorComponent;
class RenderComponent;

class Actor
{
public:
	// public
	Transform	m_Transform;

	// transform
	XMMATRIX	GetWorldTransform();

	// information
	bool	IsRoot() const;
	bool	HaveChild() const;
	bool	IsEnabled() const;
	bool	NeedTick() const;
	bool	NeedRendering() const;

	// Rendering process
	void	AttachRenderComponent(const RenderComponent * i_ComponentToCopy);	// Take the render component and make a copy for the game object
	bool	DetachRenderComponent();

	// friend class
	friend class World;

protected:
	// informations
	bool			m_Enabled;	// can be managed in the child class
	bool			m_NeedTick;

private:
	// actor creation
	Actor(World * i_World, Actor * i_Parent = nullptr);
	~Actor();

	// virtual function
	virtual void	Tick(float i_Elapsed);
	void			Render();	// render component

	// parenting system
	std::vector<Actor*>		m_Children;
	Actor *					m_Parent;
	
	// components management
	RenderComponent *		m_RenderComponent;

	// world of the actor
	World * const			m_World;
};