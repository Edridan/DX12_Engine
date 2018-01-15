#include "DX12Window.h"
#include <DirectXMath.h>
#include <windowsx.h>

#include "d3dx12.h"
#include "resource.h"

DX12Window::DX12Window(HINSTANCE i_hInstance, const wchar_t * i_WindowName, const wchar_t * i_WindowTitle, UINT i_Width, UINT i_Height, Icon i_Icon)
	:m_hInstance(i_hInstance)
	,m_Hwnd(nullptr)
	,m_Width(i_Width)
	,m_Height(i_Height)
	,m_Fullscreen(false)
	,m_Icon(nullptr)
{
	// Window class definition
	m_WindowClassX.cbSize			= sizeof(WNDCLASSEX);
	m_WindowClassX.style			= CS_HREDRAW | CS_VREDRAW;
	m_WindowClassX.lpfnWndProc		= DX12Window::WndProc;
	m_WindowClassX.cbClsExtra		= NULL;
	m_WindowClassX.cbWndExtra		= NULL;
	m_WindowClassX.hInstance		= m_hInstance;
	m_WindowClassX.hCursor			= LoadCursor(NULL, IDC_ARROW);
	m_WindowClassX.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 2);
	m_WindowClassX.lpszMenuName		= NULL;
	m_WindowClassX.lpszClassName	= i_WindowName;

	if (i_Icon.m_Resource != NULL)
	{
		// load icon
		m_Icon = LoadIcon(m_hInstance, MAKEINTRESOURCE(i_Icon.m_Resource));

		if (m_Icon)
		{
			m_WindowClassX.hIconSm = m_Icon;
			m_WindowClassX.hIcon = m_Icon;
		}
		else
		{
			MessageBox(NULL, L"Error loading icon",
				L"Error", MB_OK | MB_ICONERROR);
		}
	}


	// Setup icon
	if (m_Icon == NULL)
	{
		m_WindowClassX.hIconSm	= LoadIcon(NULL, IDI_APPLICATION);
		m_WindowClassX.hIcon	= LoadIcon(NULL, IDI_APPLICATION);

	}

	// Error handler
	if (!RegisterClassEx(&m_WindowClassX))
	{
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Create the HWND
	m_Hwnd = CreateWindowEx(NULL,
		i_WindowName,
		i_WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		i_Width, i_Height,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	if (!m_Hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Create and show the window
	ShowWindow(m_Hwnd, SW_SHOW);
	UpdateWindow(m_Hwnd);

	// Update mouse position
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// Retreive the current position of the mouse
	m_MousePosition.x = cursorPos.x;
	m_MousePosition.y = cursorPos.y;

	// Setup callback management
	SetWindowLongPtrW(m_Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	m_IsOpen = true;
}

LRESULT CALLBACK DX12Window::WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)

{
	DX12Window* window = reinterpret_cast<DX12Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				if (window != nullptr)
					window->Close();
			}
		}
		return 0;

	case WM_DESTROY: // x button on top right corner of window was pressed
		if (window != nullptr)
			window->Close();

		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE:
		window->RegisterMouseMove(IntVec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
	}

	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

void DX12Window::RegisterMouseMove(const IntVec2 & i_NewPosition)
{
	m_MouseMove = m_MousePosition - i_NewPosition;
	m_MousePosition = i_NewPosition;

	OutputDebug("Mouse move (%i,%i) [Pos = (%i,%i)]", m_MouseMove.x, m_MouseMove.y, m_MousePosition.x, m_MousePosition.y);
}

void DX12Window::Update()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void DX12Window::Close()
{
	m_IsOpen = false;
	DestroyWindow(m_Hwnd);
}

bool DX12Window::IsOpen() const
{
	return m_IsOpen;
}

UINT DX12Window::GetWidth() const
{
	return m_Width;
}

UINT DX12Window::GetHeight() const
{
	return m_Height;
}

HWND DX12Window::GetHWnd() const
{
	return m_Hwnd;
}

IntVec2 DX12Window::GetMouseMove() const
{
	return m_MouseMove;
}

DX12Window::~DX12Window()
{
	// To do : Close properly window
}