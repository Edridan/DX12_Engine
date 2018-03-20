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
		float					Range = 10.f;	// TO DO
		float					Quadratic = 0.44f;
		float					Linear = 0.35f;
		float					Constant = 1.f;
		// spot light edges
		float					Theta = 50.f;
		float					InnerCutoff = 12.5f;
		float					OuterCutoff = 12.5f;

	};

	LightComponent(const LightDesc & i_Desc, Actor * i_Actor);
	LightComponent(Actor * i_Actor);
	~LightComponent();

	// light management
	Light *					GetLight() const;
	Light::ELightType		GetLightType() const;

private:
	Light *					m_Light;	// light internal management

#ifdef WITH_EDITOR
	// Inherited via ActorComponent
	virtual void DrawUIComponentInternal() override;
#endif
};
