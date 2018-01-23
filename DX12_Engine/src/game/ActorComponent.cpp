#include "ActorComponent.h"

ActorComponent::ActorComponent(Actor * i_Actor)
	:m_Actor(i_Actor)
{
}

ActorComponent::~ActorComponent()
{
}

bool ActorComponent::IsValid() const
{
	return (m_Actor != nullptr);
}

Actor * ActorComponent::GetActor() const
{
	return m_Actor;
}
