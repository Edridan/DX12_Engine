// light component is a light caster
// this create light
// lights are rendered in the immediate context

#pragma once

#include "dx12/DX12Light.h"
#include "ActorComponent.h"

class LightComponent : public ActorComponent
{
public:
	struct LightDesc
	{
		DX12Light::ELightType		Type;
		float						Color[4];
	};

	LightComponent(const LightDesc & i_Desc, Actor * i_Actor);
	LightComponent(Actor * i_Actor);
	~LightComponent();

	// light management
	DX12Light *		GetLight() const;

private:
	DX12Light *			m_Light;	// light internal management

#ifdef WITH_EDITOR
	// Inherited via ActorComponent
	virtual void DrawUIComponentInternal() override;
#endif
};
