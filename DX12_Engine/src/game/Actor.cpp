#include "Actor.h"

bool Actor::IsRoot() const
{
	return (m_Parent == nullptr);
}

bool Actor::HaveChild() const
{
	return (m_Children.size() != 0);
}

Actor::Actor(World * i_World, Actor * i_Parent)
	:m_Children()
	,m_World(i_World)
	,m_Parent(nullptr)
	,m_Transform()
{
}

Actor::~Actor()
{
}

void Actor::Tick(float i_Elapsed)
{
}
