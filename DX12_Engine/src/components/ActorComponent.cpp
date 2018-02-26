#include "ActorComponent.h"

#include <string.h>

ActorComponent::ActorComponent(Actor * i_Actor, const char * i_ComponentName)
	:m_Actor(i_Actor)
	,m_Enabled(true)
{
	// retreive the name of the component
	strcpy_s(m_ComponentName, i_ComponentName);
}

ActorComponent::~ActorComponent()
{
}

bool ActorComponent::IsValid() const
{
	return (m_Actor != nullptr);
}

bool ActorComponent::IsEnabled() const
{
	return m_Enabled;
}

Actor * ActorComponent::GetActor() const
{
	return m_Actor;
}

void ActorComponent::SetEnabled(bool i_Enabled)
{
	m_Enabled = i_Enabled;
}


////////////////////////////////////////////
// editor only
#ifdef WITH_EDITOR
// Imgui include
#include "ui/UI.h"

void ActorComponent::DrawUIComponent()
{
	// first we add a separator
	ImGui::Separator();

	bool isOpen = ImGui::TreeNode(m_ComponentName);

	if (isOpen)
	{
		// checkbox for enabling the component
		ImGui::Checkbox("Is Enabled", &m_Enabled);

		DrawUIComponentInternal();

		// we finished to render the component
		ImGui::TreePop();
	}
}
#endif