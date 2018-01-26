// ui layer management
// this manage hud between the engine/game and Imgui lib

#pragma once

#include <Windows.h>

#include "dx12/d3dx12.h"

// class predef
class Window;

class UILayer
{
public:
	struct LayerStyleDesc
	{

	};

	UILayer(Window * i_Window);	// Imgui initialization
	UILayer(Window * i_Window, const LayerStyleDesc & i_Style);
	~UILayer();

	// ui global management
	void		SetEnable(const bool i_Enable);	// enable/disable ui layer

	// information
	bool		IsEnable() const;

	// friend class
	friend class Engine;
private:
	// management
	void		BeginNewFrame();
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);

	// imgui input management
	static LRESULT CALLBACK UpdateImguiInput(HWND i_Window, UINT i_Msg, WPARAM i_wParam, LPARAM i_lParam);	// callback for imgui event


	// layer management
	bool		m_Enabled;
	
	// imgui management
	HWND		m_Handle;
	bool		m_Initialized;
};
