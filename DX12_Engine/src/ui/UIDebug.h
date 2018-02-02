#pragma once

#include "ui/UIWindow.h"
#include <DirectXMath.h>

class Engine;

class UIDebug : public UIWindow
{
public:
	UIDebug();
	~UIDebug();
private:

	// Inherited via UIWindow
	virtual void DrawWindow() override;

	// direct pointer
	DirectX::XMFLOAT4 *		m_CameraPos;
	
	// engine
	Engine *		m_Engine;

	// Updatable values
	int 			m_FramePerSecs;
	float 			m_AppTime;
};