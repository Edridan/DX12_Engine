#include "LightComponent.h"


LightComponent::LightComponent(Actor * i_Actor)
	:ActorComponent(i_Actor, "Light Component")
{
}

LightComponent::~LightComponent()
{
}

#ifdef WITH_EDITOR
void LightComponent::DrawUIComponentInternal()
{

}
#endif