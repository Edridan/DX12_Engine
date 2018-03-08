// light component is a light caster
// this create light
// lights are rendered in the immediate context

#pragma once

#include "engine/Light.h"
#include "ActorComponent.h"

class LightComponent : public ActorComponent
{
public:
	struct LightDesc
	{
		Light::ELightType		Type = Light::ePointLight;
		DirectX::XMFLOAT4		Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		float					Range = 10.f;
	};

	LightComponent(const LightDesc & i_Desc, Actor * i_Actor);
	LightComponent(Actor * i_Actor);
	~LightComponent();

	// light management
	Light::LightData *		GetLightData() const;
	Light::ELightType		GetLightType() const;

private:
	Light::LightData *		m_LightData;	// light internal management
	Light::ELightType		m_Type;			// type of the light

#ifdef WITH_EDITOR
	// Inherited via ActorComponent
	virtual void DrawUIComponentInternal() override;
#endif
};
