// material builder editor
// this allow to edit and build a material

#pragma once

#include "engine/Defines.h"


#ifdef WITH_EDITOR

#include "ui/UIWindow.h"

class UIMaterialBuilder : public UIWindow
{
public:
	UIMaterialBuilder();
	~UIMaterialBuilder();

	// window management

private:

	// Inherited via UIWindow
	virtual void DrawWindow() override;

};



#endif