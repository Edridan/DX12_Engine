// an actor component is a component that can be attached to an actor

#pragma once

// class predef
class Actor;

class ActorComponent
{
public:
	ActorComponent(Actor * i_Actor);
	~ActorComponent();

	// informations
	Actor *		GetActor() const;

protected:
	Actor *		m_Actor;
};