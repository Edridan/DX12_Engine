#include "LightComponent.h"

using namespace Light;

LightComponent::LightComponent(const LightDesc & i_Desc, Actor * i_Actor)
	:ActorComponent(i_Actor, "Light Component")
{
	// generate light
	m_LightData = new LightData;

	// create default structure
	m_Type						= i_Desc.Type;
	m_LightData->DiffuseColor	= i_Desc.Color;
	m_LightData->Range			= i_Desc.Range;
}

LightComponent::LightComponent(Actor * i_Actor)
	:ActorComponent(i_Actor, "Light Component")
{
	// generate light
	m_LightData = new LightData;

	// create default structure
	m_Type						= Light::ELightType::ePointLight;
	m_LightData->DiffuseColor	= DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_LightData->Range			= 10.f;
}

LightComponent::~LightComponent()
{
}

Light::LightData * LightComponent::GetLightData() const
{
	return m_LightData;
}

Light::ELightType LightComponent::GetLightType() const
{
	return m_Type;
}

#ifdef WITH_EDITOR
#include "ui/UI.h"
void LightComponent::DrawUIComponentInternal()
{
	static int selectedType = (int)GetLightType();
	static const char * typesName[] = { "Point Light", "Spot Light", "Directionnal" };
	DirectX::XMFLOAT4 lightColor = m_LightData->DiffuseColor;
	float color[3] = { lightColor.x,lightColor.y, lightColor.z };

	// type of the color
	ImGui::Combo("Type", &selectedType, typesName, ELightType::eCount);
	
	// light global data
	// color management
	ImGui::ColorEdit3("Color", color);
	if (color[0] != lightColor.x || color[1] != lightColor.y || color[2] != lightColor.z)
	{
		lightColor.x = color[0];
		lightColor.y = color[1];
		lightColor.z = color[2];
		m_LightData->DiffuseColor = lightColor;
	}

	switch (selectedType)
	{
	case 0:		// point light
		ImGui::SliderFloat("Range", &m_LightData->Range, 0.5f, 1000.f);
		break;
	case 1:		// spot light
		ImGui::SliderFloat("Range", &m_LightData->Range, 0.5f, 1000.f);

		break;
	case 2:		// directionnal light
		break;
	}
	

}
#endif