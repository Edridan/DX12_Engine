#include "UIWindow.h"

#include "../lib/imgui-d3d12/imgui.h"

unsigned int UIWindow::s_Instance = 0;

UIWindow::UIWindow(const std::string & i_Name, int i_Flags /*= UIWindowFlags::eNone*/)
	:m_WindowName(i_Name)
	,m_Id(s_Instance++)
	,m_Active(true)
	,m_WindowFlags(i_Flags)
{

}

UIWindow::~UIWindow()
{
}

bool UIWindow::IsActive() const
{
	return false;
}

void UIWindow::SetActive(bool i_Active)
{
	bool isOpen = m_Active;
	m_Active = i_Active;
	
	// callback management
	if (isOpen && !m_Active)
	{
		OnClose(true);
	}
}

unsigned int UIWindow::GetId() const
{
	return m_Id;
}

const char * UIWindow::GetName() const
{
	return m_WindowName.c_str();
}

bool UIWindow::IsFocused() const
{
	return (m_Active && m_Focused);
}

void UIWindow::StartDraw()
{
	bool active = m_Active;

	ImGui::Begin(m_WindowName.c_str(), &m_Active, m_WindowFlags);

	// update data for the current window
	m_Focused = ImGui::IsWindowFocused();

	if (active != m_Active && (!m_Active))
	{
		// callback
		OnClose(false);
	}
}

void UIWindow::EndDraw() const
{
	ImGui::End();
}

void UIWindow::OnClose(bool i_UserCall)
{
	// do nothing, close has been called by neither the user or ImGui
}
