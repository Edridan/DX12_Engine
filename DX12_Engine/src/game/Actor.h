// this is a base class for an actor
// erverything that can add to a world (Meshes, Characters, Door, Props) are actors
// you can inherit from this class if you need to add behavior to the actor
// an actor can attach itself components

#pragma once

#include <vector>
#include <string>

#include "engine/Transform.h"
#include "engine/Defines.h"
#include "RenderComponent.h"

// class predef
class World;	// world of the actor

class Actor
{
public:
	// this initialize an actor depending a basic description
	struct ActorDesc
	{
		// actor definition
		std::wstring Name			= L"Unnamed Actor";
		UINT64 Id					= (UINT64)-1;		// if (-1) no predefined id
		// actor logic
		bool NeedTick				= false;
		// actor rendering
		std::wstring Mesh			= L"";
		UINT SubMeshId				= (UINT)-1;
	};

	// public
	Transform	m_Transform;

	// transform
	XMMATRIX	GetWorldTransform();

	// information
	bool	IsRoot() const;
	bool	HaveChild() const;
	bool	IsEnabled() const;
	bool	NeedTick() const;
	bool	IsHidden() const;
	bool	NeedRendering() const;

	UINT	ChildCount() const;
	Actor *	GetChild(UINT i_Index) const;

	// actor specs
	UINT64					GetId() const;
	const std::wstring &	GetName() const;
	World *					GetWorld() const;

	// rendering process (manage attach and detach render component)
	void				AttachRenderComponent(const RenderComponent::RenderComponentDesc & i_ComponentDesc);	// Take the render component and make a copy for the game object
	bool				DetachRenderComponent();
	RenderComponent *	GetRenderComponent() const;


	// editor purpose
#ifdef WITH_EDITOR
	void	SetName(const std::wstring & i_NewName);
#endif

	// friend class
	friend class World;

protected:
	// informations
	bool			m_Enabled;	// can be managed in the child class
	bool			m_NeedTick;
	bool			m_Hidden;

private:
	// actor creation
	Actor(const ActorDesc & i_Desc, World * i_World);
	Actor(World * i_World);	// this constructor is used for special Actors (overriden actors that don't need description)
	~Actor();

	// virtual function
	virtual void	Tick(float i_Elapsed);
	// events
	virtual void	Created();		// called right after creation
	virtual void	Destroyed();	// called just before deletion

	// specific render informations
	void			Render();	// render component

	// parenting system
	std::vector<Actor*>		m_Children;
	Actor *					m_Parent;

	// information
	std::wstring			m_Name;
	UINT64					m_Id;
	
	// components management
	RenderComponent *		m_RenderComponent;

	// world of the actor
	World * const			m_World;
};