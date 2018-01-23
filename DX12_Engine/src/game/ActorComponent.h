// an actor component is a component that can be attached to an actor
// a component is only data, it contains almost no logic (some logic are required for some components)
// this can be used for rendering objects, attaching sockets or some AI behavior state or setup holder
// the logic for actors are defined into the actors directly (by overriding them)
#pragma once

// class predef
class Actor;

// this is a base class for a component
class ActorComponent
{
public:
	ActorComponent(Actor * i_Actor);
	~ActorComponent();

	// informations
	bool		IsValid() const;	// return false if the component is not attached to an actor
	Actor *		GetActor() const;

protected:
	Actor *		m_Actor;
};