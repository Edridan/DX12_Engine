// ui layer management
// this manage hud between the engine/game and Imgui lib

#pragma once

#include <Windows.h>
#include <vector>

#include "dx12/d3dx12.h"

// class predef
class Window;
class UIWindow;

class UILayer
{
public:
	struct LayerStyleDesc
	{
		// global ui style
		float Alpha				= 10.0f;
		bool AntiAliasedLines	= true;
		bool AntiAliasedShapes	= true;
		// window style
		float WindowRounding	= 0.0f;


	};

	UILayer(Window * i_Window);	// Imgui initialization
	UILayer(Window * i_Window, const LayerStyleDesc & i_Style);
	~UILayer();

	// ui global management
	void		SetEnable(const bool i_Enable);	// enable/disable ui layer
	void		SetUIStyle(const LayerStyleDesc & i_Style);
	void		SetAlpha(const float i_Alpha);

	// information
	bool		IsEnable() const;

	// window management
	unsigned int	PushUIWindowOnLayer(UIWindow * i_Window);
	void			PopUIWindowFromLayer(UIWindow * i_Window);
	void			PopUIWindowFromLayer(unsigned int i_Id);
	size_t			GetWindowCount() const;

	// friend class
	friend class Engine;
private:
	// management
	void		DisplayUIOnLayer();	// call this before PushOnCommandList
	void		PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList);

	// imgui input management
	static LRESULT CALLBACK UpdateImguiInput(HWND i_Window, UINT i_Msg, WPARAM i_wParam, LPARAM i_lParam);	// callback for imgui event

	// layer management
	bool					m_Enabled;
	std::vector<UIWindow*>	m_Windows;
	
	// imgui management
	HWND		m_Handle;
	bool		m_Initialized;
};
