#include "UILayer.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12ImGui.h"
#include "engine/Window.h"
#include "engine/Utils.h"
#include "engine/Debug.h"
#include "ui/UIWindow.h"

// imgui
#include "../lib/imgui-d3d12/imgui.h"

UILayer::UILayer(Window * i_Window)
	:m_Handle(i_Window->GetHWnd())
	,m_Initialized(false)
	,m_Enabled(true)
{
	ImGuiIO & io = ImGui::GetIO();

	// Keyboard mapping ImGui will use those indices to peek into the io
	// KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;

	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	// setup imgui
	io.RenderDrawListsFn = ImGuiD3D12::RenderDrawListImGui;	// callback for rendering
	io.ImeWindowHandle = i_Window->GetHWnd();

	if (FAILED(ImGuiD3D12::InitializeDX12ImGui()))
	{
		DEBUG_BREAK;
		return;
	}

	// register callback input
	i_Window->RegisterInputCallback(& UILayer::UpdateImguiInput);

	// the imgui is initialized
	m_Initialized = true;

	// set the default
	LayerStyleDesc desc;
	SetUIStyle(desc);
}

UILayer::UILayer(Window * i_Window, const LayerStyleDesc & i_Style)
	:UILayer(i_Window)
{
	// manage style
	if (!m_Initialized) return;

	SetUIStyle(i_Style);
}

UILayer::~UILayer()
{
}

void UILayer::SetEnable(const bool i_Enable)
{
	m_Enabled = i_Enable;
}

void UILayer::SetUIStyle(const LayerStyleDesc & i_Style)
{
	// imgui reskin
	ImGuiStyle & style = ImGui::GetStyle();

	style.WindowRounding = i_Style.WindowRounding;
	style.Alpha = i_Style.Alpha;
	// set colors
	style.Colors[ImGuiCol_WindowBg]			= ImVec4(0.11f, 0.11f, 0.11f, 1.f);
	style.Colors[ImGuiCol_TitleBg]			= ImVec4(0.31f, 0.07f, 0.01f, 1.f);
	style.Colors[ImGuiCol_TitleBgActive]	= ImVec4(0.61f, 0.23f, 0.15f, 1.f);
	style.Colors[ImGuiCol_FrameBg]			= ImVec4(0.21f, 0.21f, 0.21f, 1.f);
}

void UILayer::SetAlpha(const float i_Alpha)
{
	ImGuiStyle & style = ImGui::GetStyle();
	style.Alpha = i_Alpha;
}

bool UILayer::IsEnable() const
{
	return m_Enabled;
}

bool UILayer::IsOneWindowFocused() const
{
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		if (m_Windows[i]->IsFocused())
			return true;
	}

	// no window is focused
	return false;
}

unsigned int UILayer::PushUIWindowOnLayer(UIWindow * i_Window)
{
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		if (m_Windows[i] == i_Window)
		{
			PRINT_DEBUG("Error, window \"%s\"[%i] is already added to the layer!\n", i_Window->GetName(), i_Window->GetId());
			DEBUG_BREAK;
			return (unsigned int)-1;	// return broken id
		}
	}

	m_Windows.push_back(i_Window);
}

void UILayer::PopUIWindowFromLayer(UIWindow * i_Window)
{
	auto itr = m_Windows.begin();

	while (itr != m_Windows.end())
	{
		if ((*itr) == i_Window)
		{
			m_Windows.erase(itr);
			return;
		}

		++itr;
	}

	PRINT_DEBUG("Error, window \"%s\"[%i] is not in the layer!\n", i_Window->GetName(), i_Window->GetId());
	DEBUG_BREAK;
}

void UILayer::PopUIWindowFromLayer(unsigned int i_Id)
{
	auto itr = m_Windows.begin();

	while (itr != m_Windows.end())
	{
		if ((*itr)->GetId() == i_Id)
		{
			m_Windows.erase(itr);
			return;
		}

		++itr;
	}

	PRINT_DEBUG("Error, window [%i] is not in the layer!\n", i_Id);
	DEBUG_BREAK;
}

size_t UILayer::GetWindowCount() const
{
	return m_Windows.size();
}

UIWindow * UILayer::FindUIWindowByName(const std::string & i_Name)
{
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		if (m_Windows[i]->GetName() == i_Name)
		{
			return m_Windows[i];
		}
	}

	// we didn't find the window
	return nullptr;
}

UIWindow * UILayer::FindUIWindowById(unsigned int i_Id)
{
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		if (m_Windows[i]->GetId() == i_Id)
		{
			return m_Windows[i];
		}
	}

	// we didn't find the window
	return nullptr;
}

void UILayer::DisplayUIOnLayer()
{
	ImGuiIO & io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect(m_Handle, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;

	ImVec2 pos = io.MousePos;

	// hide OS mouse cursor if ImGui is drawing it
	SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));

	// start the frame
	ImGui::NewFrame();

	// we create vertices for the UI
	for (size_t i = 0; i < m_Windows.size(); ++i)
	{
		// create vertices for the window
		m_Windows[i]->StartDraw();
		m_Windows[i]->DrawWindow();
		m_Windows[i]->EndDraw();
	}
}

void UILayer::PushOnCommandList(ID3D12GraphicsCommandList * i_CommandList)
{
	if (m_Enabled)
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();
		ImGuiD3D12::SetRenderDataImGui(i_CommandList, render.GetRenderTarget());
		ImGui::Render();	// call to render
	}
}

LRESULT UILayer::UpdateImguiInput(HWND i_Window, UINT i_Msg, WPARAM i_wParam, LPARAM i_lParam)
{
	ImGuiIO & io = ImGui::GetIO();

	// update imgui input
	switch (i_Msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;

	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;

	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;

	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;

	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(i_wParam) > 0 ? +1.0f : -1.0f;
		return true;

	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(i_lParam);
		io.MousePos.y = (signed short)(i_lParam >> 16);
		return true;

	case WM_KEYDOWN:
		if (i_wParam < 256)
			io.KeysDown[i_wParam] = 1;
		return true;

	case WM_KEYUP:
		if (i_wParam < 256)
			io.KeysDown[i_wParam] = 0;
		return true;

	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (i_wParam > 0 && i_wParam < 0x10000)
			io.AddInputCharacter((unsigned short)i_wParam);
		return true;
	}

	return LRESULT(0);
}
