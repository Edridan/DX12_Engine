// console UI window


#pragma once

#include "UIWindow.h"

class UIConsole : public UIWindow
{
public:
	UIConsole();
	~UIConsole();

private:
	std::string		m_Buffer;

	// Inherited via UIWindow
	virtual void DrawWindow() override;
	// buffer text for rendering on the window
};