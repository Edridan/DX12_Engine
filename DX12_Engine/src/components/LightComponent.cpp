#include "LightComponent.h"


LightComponent::LightComponent(LightDesc & i_Desc, Actor * i_Actor)
	:ActorComponent(i_Actor, "Light Component")
{
	// generate light
	SetColor(i_Desc.Color);
	SetType(i_Desc.Type);
}

LightComponent::LightComponent(Actor * i_Actor)
	:ActorComponent(i_Actor, "Light Component")
{
}

LightComponent::~LightComponent()
{
}

LightComponent::LightType LightComponent::GetType() const
{
	return m_Type;
}

const float * LightComponent::GetColor() const
{
	return m_Color;
}

inline void LightComponent::SetColor(const float i_Color[])
{
	// retreive the color
	for (size_t i = 0; i < 4; ++i)
	{
		m_Color[i] = i_Color[i];
	}
}

void LightComponent::SetType(LightType i_Type)
{
	m_Type = i_Type;
}

#ifdef WITH_EDITOR
#include "ui/UI.h"
void LightComponent::DrawUIComponentInternal()
{
	static int selectedType = 0;
	static const char * typesName[] = { "Point Light", "Spot Light", "Directionnal" };

	ImGui::Combo("Type", &selectedType, typesName, eLightTypeCount);
	ImGui::ColorEdit3("Color", m_Color);
}
#endif