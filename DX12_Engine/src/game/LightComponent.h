// light component definition
// this manage and create lights
// lights are rendered in the immediate context

#pragma once

#include "ActorComponent.h"

class LightComponent : public ActorComponent
{
public:
	LightComponent(Actor * i_Actor);
	~LightComponent();


private:

#ifdef WITH_EDITOR
	// Inherited via ActorComponent
	virtual void DrawUIComponentInternal() override;
#endif
};
