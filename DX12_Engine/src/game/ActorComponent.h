// an actor component is a component that can be attached to an actor

#pragma once

// class predef
class Actor;

class ActorComponent
{
public:
	ActorComponent();
	~ActorComponent();

private:
	Actor *		m_Actor;
};