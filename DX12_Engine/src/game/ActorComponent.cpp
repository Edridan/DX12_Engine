#include "ActorComponent.h"

ActorComponent::ActorComponent(Actor * i_Actor)
	:m_Actor(i_Actor)
{
}

ActorComponent::~ActorComponent()
{
}

Actor * ActorComponent::GetActor() const
{
	return m_Actor;
}
