#include "Actor.h"

#include "RenderComponent.h"

XMMATRIX Actor::GetWorldTransform()
{
	XMMATRIX thisMat = XMLoadFloat4x4(&m_Transform.GetMatrix());

	if (m_Parent != nullptr)
	{
		thisMat = m_Parent->GetWorldTransform() * thisMat;
	}
	
	return thisMat;
}

bool Actor::IsRoot() const
{
	return (m_Parent == nullptr);
}

bool Actor::HaveChild() const
{
	return (m_Children.size() != 0);
}

bool Actor::IsEnabled() const
{
	return m_Enabled;
}

bool Actor::NeedTick() const
{
	return m_NeedTick;
}

bool Actor::NeedRendering() const
{
	return (m_RenderComponent != nullptr);
}

void Actor::AttachRenderComponent(const RenderComponent * i_Component)
{
	m_RenderComponent = new RenderComponent;
}

bool Actor::DetachRenderComponent()
{
	return false;
}

Actor::Actor(World * i_World, Actor * i_Parent)
	:m_Children()
	,m_World(i_World)
	,m_Parent(nullptr)
	,m_Transform()
	,m_Enabled(true)
	,m_NeedTick(false)
	// components
	,m_RenderComponent(nullptr)
{
}

Actor::~Actor()
{
}

void Actor::Tick(float i_Elapsed)
{
}

void Actor::Render()
{

}
