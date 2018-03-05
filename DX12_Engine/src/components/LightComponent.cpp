#include "LightComponent.h"


LightComponent::LightComponent(const LightDesc & i_Desc, Actor * i_Actor)
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

DX12Light * LightComponent::GetLight() const
{
	return m_Light;
}

#ifdef WITH_EDITOR
#include "ui/UI.h"
void LightComponent::DrawUIComponentInternal()
{
	static int selectedType = (int)m_Light->GetType();
	static const char * typesName[] = { "Point Light", "Spot Light", "Directionnal" };
	DirectX::XMFLOAT4 lightColor = m_Light->GetColor();
	float color[3] = { lightColor.x,lightColor.y, lightColor.z };

	// type of the color
	ImGui::Combo("Type", &selectedType, typesName, DX12Light::ELightType::eCount);
	// color management
	ImGui::ColorEdit3("Color", color);
	if (color[0] != lightColor.x || color[1] != lightColor.y || color[2] != lightColor.z)
	{
		lightColor.x = color[0];
		lightColor.y = color[1];
		lightColor.z = color[2];
		m_Light->SetColor(lightColor);
	}

}
#endif