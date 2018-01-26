// DX 12 window

#pragma once

#include <vector>
#include <functional>
#include <Windows.h>
#include "dx12/DX12Utils.h"

class Window
{
public:
	// Struct def
	struct Icon
	{
		// Constructor
		Icon(LPWSTR i_Resource, UINT i_Width, UINT i_Height)
			:m_Height(i_Height)
			,m_Width(i_Width)
			,m_Resource(i_Resource)
		{}

		LPWSTR		m_Resource;
		UINT		m_Height, m_Width;
	};

	// Window
	Window(HINSTANCE i_hInstance, const wchar_t * i_WindowName = L"Default", const wchar_t * i_WindowTitle = L"Default", UINT i_Width = 800, UINT i_Height = 600, Icon i_Icon = Icon(L"Default", 0, 0));
	~Window();
	
	// Windows management
	void	Update();
	void	Close();
	bool	IsOpen() const;

	// Get/Set
	UINT	GetWidth() const;
	UINT	GetHeight() const;
	IntVec2	GetSize() const;
	IntVec2	GetBackSize() const;
	HWND	GetHWnd() const;

	// input management
	IntVec2	GetMouseMove() const;
	IntVec2 GetResize() const;
	bool	HasBeenResized() const;

	// callback definition
	typedef std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> InputFunc;
	void	RegisterInputCallback(const InputFunc & i_Callback);

private:

	// Static
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// internal management
	void		RegisterMouseMove(const IntVec2 & i_NewPosition);
	void		RegisterResize(const IntVec2 & i_Resize);
	void		CallInputCallbackFunc(HWND i_Window, UINT i_Msg, WPARAM i_wParam, LPARAM i_lParam);

	// Window
	HICON		m_Icon;
	HWND		m_Hwnd;
	HINSTANCE	m_hInstance;
	WNDCLASSEX	m_WindowClassX;
	UINT		m_Width;
	UINT		m_Height;
	bool		m_Fullscreen;
	bool		m_IsOpen;

	// callback inputs
	std::vector<InputFunc>	m_Callbacks;

	// input
	IntVec2		m_MousePosition;
	IntVec2		m_MouseMove;	// movement of the mouse on this frame
	// resize event
	bool		m_HasBeenResized;
	IntVec2		m_Resized;
};
