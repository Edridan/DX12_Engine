// material builder editor
// this allow to edit and build a material

#pragma once

#include "engine/Engine.h"
#include "ui/UIWindow.h"

#ifdef WITH_EDITOR

class UIMaterialBuilder : public UIWindow
{
public:
	UIMaterialBuilder();
	~UIMaterialBuilder();

private:


	// Inherited via UIWindow
	virtual void DrawWindow() override;

};



#endif