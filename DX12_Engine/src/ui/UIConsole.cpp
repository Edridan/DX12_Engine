#include "UIConsole.h"

#include "UI.h"

UIConsole::UIConsole()
	:UIWindow("Console", UIWindow::eNoResize)
{
}

UIConsole::~UIConsole()
{
}


void UIConsole::DrawWindow()
{
	ImGui::BeginGroup();

	ImGui::EndGroup();
}
