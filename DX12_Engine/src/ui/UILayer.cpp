#include "UILayer.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12Utils.h"
#include "dx12/DX12ImGui.h"
#include "engine/Window.h"


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
}

UILayer::UILayer(Window * i_Window, const LayerStyleDesc & i_Style)
	:UILayer(i_Window)
{
	// manage style
	if (!m_Initialized) return;

	// imgui reskin
	ImGuiStyle & style = ImGui::GetStyle();

	style.WindowRounding = 0.f;
	// set colors
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.f);
}

UILayer::~UILayer()
{
}

void UILayer::SetEnable(const bool i_Enable)
{
	m_Enabled = i_Enable;
}

bool UILayer::IsEnable() const
{
	return m_Enabled;
}

void UILayer::BeginNewFrame()
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

	// Hide OS mouse cursor if ImGui is drawing it
	SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));

	// Start the frame
	ImGui::NewFrame();
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
