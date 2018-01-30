// this window is for debug purpose


#pragma once

#include "ui/UIWindow.h"

// class predef
class DX12Texture;

class UITexture : public UIWindow
{
public:
	UITexture();
	~UITexture();

	// manage the window
	void SetTexture(const DX12Texture * i_Texture);

private:
	// Inherited via UIWindow
	virtual void DrawWindow() override;

	// texture
	const DX12Texture *		m_Texture;
};