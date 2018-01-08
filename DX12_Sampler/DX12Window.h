// DX 12 window

#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

class DX12Window
{
public:
	DX12Window(HINSTANCE i_hInstance, const wchar_t * i_WindowName = L"Default", const wchar_t * i_WindowTitle = L"Default", UINT i_Width = 800, UINT i_Height = 600);
	~DX12Window();
	
	// Windows management
	void	Update();
	void	Close();
	bool	IsOpen() const;

	// Get/Set
	UINT	GetWidth() const;
	UINT	GetHeight() const;
	HWND	GetHWnd() const;

private:

	// Static
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Window
	HWND		m_Hwnd;
	HINSTANCE	m_hInstance;
	WNDCLASSEX	m_WindowClassX;
	UINT		m_Width;
	UINT		m_Height;
	bool		m_Fullscreen;
	bool		m_IsOpen;
};
