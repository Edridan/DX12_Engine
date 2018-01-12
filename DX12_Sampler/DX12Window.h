// DX 12 window

#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include "DX12Utils.h"

class DX12Window
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

	// DX12Window
	DX12Window(HINSTANCE i_hInstance, const wchar_t * i_WindowName = L"Default", const wchar_t * i_WindowTitle = L"Default", UINT i_Width = 800, UINT i_Height = 600, Icon i_Icon = Icon(L"Default", 0, 0));
	~DX12Window();
	
	// Windows management
	void	Update();
	void	Close();
	bool	IsOpen() const;

	// Get/Set
	UINT	GetWidth() const;
	UINT	GetHeight() const;
	HWND	GetHWnd() const;

	// input management
	IntVec2	GetMouseMove() const;

private:

	// Static
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// internal management
	void		RegisterMouseMove(const IntVec2 & i_Movement);

	// Window
	HICON		m_Icon;
	HWND		m_Hwnd;
	HINSTANCE	m_hInstance;
	WNDCLASSEX	m_WindowClassX;
	UINT		m_Width;
	UINT		m_Height;
	bool		m_Fullscreen;
	bool		m_IsOpen;

	// input
	IntVec2		m_MousePosition;
	IntVec2		m_MouseMove;	// movement of the mouse on this frame
};
