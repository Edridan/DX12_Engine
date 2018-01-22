// this is used for rendering object as mesh
// this is attached to an actor

#pragma once

#include "ActorComponent.h"

class RenderComponent : public ActorComponent
{
public:
	// define the render pass of the component
	enum RenderPass
	{
		eOpaqueGeometry,
		eSemiTransparent,

	};

	RenderComponent();
	RenderComponent(const RenderComponent * i_Other);
	~RenderComponent();

private:

};