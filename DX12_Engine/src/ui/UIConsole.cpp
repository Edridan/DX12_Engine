#include "UIConsole.h"

#include "ui/UI.h"
#include "engine/Engine.h"
#include "engine/Console.h"

#define BUFFER_SIZE		2048

UIConsole::UIConsole()
	:UIWindow("Console", UIWindow::eNoCollapse)
	,m_BufferSize(1024)
{
	m_History.reserve(HISTORY_BUFFER_SIZE);
	m_CommandBuffer = new char[1024];
}

UIConsole::~UIConsole()
{
}

void UIConsole::Print(const char * i_Text)
{
	std::string strBuffer = i_Text;
	strBuffer.push_back('\n');	// push back end line at the end of the buffer

	if (m_History.size() + strBuffer.size() >= m_History.capacity())
	{
		// erase the first line of the history to save memory
		auto itr = m_History.begin();

		while ((*itr) != '\n')
		{
			++itr;
		}

		m_History.erase(m_History.begin(), itr);
	}

	// append the print to the str buffer
	m_History.append(strBuffer);
}

void UIConsole::Clear()
{
	m_History.clear();
}

void UIConsole::DrawWindow()
{
	ImGui::BeginGroup();
	// text
	ImGui::BeginChild("", ImVec2(0.f, 250.f), true, 0);
	ImGui::Text(m_History.c_str());
	ImGui::EndChild();

	ImGui::InputText("", m_CommandBuffer, m_BufferSize, 0);
	ImGui::SameLine();
	if (ImGui::Button("Exec", ImVec2(64.f, 20.f)))
	{
		PushCommandOnConsole();
	}

	ImGui::EndGroup();
}

inline void UIConsole::PushCommandOnConsole()
{
	// push the command from the command buffer to the console
	Engine::GetInstance().GetConsole()->PushCommand(m_CommandBuffer);
}

void UIConsole::StaticPrint(const char * i_Text, void * i_Data)
{
	// callback for print
	((UIConsole*)i_Data)->Print(i_Text);
}