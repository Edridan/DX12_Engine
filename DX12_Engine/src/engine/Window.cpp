#include "Window.h"

#include "dx12/DX12ImGui.h"
#include "dx12/d3dx12.h"
#include <DirectXMath.h>
#include <windowsx.h>
#include <functional>



Window::Window(HINSTANCE i_hInstance, const wchar_t * i_WindowName, const wchar_t * i_WindowTitle, UINT i_Width, UINT i_Height, Icon i_Icon)
	:m_hInstance(i_hInstance)
	,m_Hwnd(nullptr)
	,m_Width(i_Width)
	,m_Height(i_Height)
	,m_Fullscreen(false)
	,m_Icon(nullptr)
	,m_Callbacks()
{
	// Window class definition
	m_WindowClassX.cbSize			= sizeof(WNDCLASSEX);
	m_WindowClassX.style			= CS_HREDRAW | CS_VREDRAW;
	m_WindowClassX.lpfnWndProc		= Window::WndProc;
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
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
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

LRESULT CALLBACK Window::WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)

{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (window != nullptr)
	{
		// call each callback registered with the window
		window->CallInputCallbackFunc(hwnd, msg, wParam, lParam);
	}

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

		// internal registration
	case WM_MOUSEMOVE:
		if (window != nullptr)
			window->RegisterMouseMove(IntVec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;

	case WM_SIZE:
		if (window != nullptr)
		{
			IntVec2 resize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			window->RegisterResize(resize);
		}
		break;
	}

	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

void Window::RegisterMouseMove(const IntVec2 & i_NewPosition)
{
	m_MouseMove = m_MousePosition - i_NewPosition;
	m_MousePosition = i_NewPosition;
}

void Window::RegisterResize(const IntVec2 & i_Resize)
{
	m_HasBeenResized = true;
	m_Width += i_Resize.x;
	m_Height += i_Resize.y;
}

void Window::CallInputCallbackFunc(HWND i_Window, UINT i_Msg, WPARAM i_wParam, LPARAM i_lParam)
{
	// call each callback registered
	for (size_t i = 0; i < m_Callbacks.size(); ++i)
	{
		(m_Callbacks[i])(i_Window, i_Msg, i_wParam, i_lParam);
	}
}

void Window::Update()
{
	// reset state
	m_MouseMove = IntVec2(0, 0);
	m_Resized = IntVec2(0, 0);

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

void Window::Close()
{
	m_IsOpen = false;
	DestroyWindow(m_Hwnd);
}

bool Window::IsOpen() const
{
	return m_IsOpen;
}

UINT Window::GetWidth() const
{
	return m_Width;
}

UINT Window::GetHeight() const
{
	return m_Height;
}

IntVec2 Window::GetSize() const
{
	return IntVec2(m_Width, m_Height);
}

IntVec2 Window::GetBackSize() const
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	return IntVec2(
		rect.right - rect.left, 
		rect.bottom - rect.top
	);
}

HWND Window::GetHWnd() const
{
	return m_Hwnd;
}

IntVec2 Window::GetMouseMove() const
{
	return m_MouseMove;
}

IntVec2 Window::GetResize() const
{
	return m_Resized;
}

bool Window::HasBeenResized() const
{
	return m_HasBeenResized;
}

void Window::RegisterInputCallback(const InputFunc & i_Callback)
{
	// push back the callback
	m_Callbacks.push_back(i_Callback);
}

Window::~Window()
{
	// close the window if needed
	if (IsOpen())	Close();

	// clear callbacks vector (optionnal)
	m_Callbacks.clear();
}