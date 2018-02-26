// light component is a light caster
// this create light
// lights are rendered in the immediate context

#pragma once

#include "ActorComponent.h"

class LightComponent : public ActorComponent
{
public:
	// enums
	enum LightType
	{
		ePointLight,
		eSpotLight,
		eDirectonnal,

		eLightTypeCount
	};

	struct LightDesc
	{
		LightType		Type;
		float			Color[4];
	};

	LightComponent(const LightDesc & i_Desc, Actor * i_Actor);
	LightComponent(Actor * i_Actor);
	~LightComponent();

	// lights defines
	LightType			GetType() const;
	const float *		GetColor() const;

	// management
	void				SetColor(const float i_Color[]);
	void				SetType(LightType i_Type);

private:

	// light manager
	LightType		m_Type;
	float			m_Color[4];

#ifdef WITH_EDITOR
	// Inherited via ActorComponent
	virtual void DrawUIComponentInternal() override;
#endif
};
