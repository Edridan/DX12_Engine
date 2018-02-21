#include "UIDebug.h"

#include "ui/UI.h"
#include "engine/Engine.h"
#include "game/World.h"
#include "game/Camera.h"

UIDebug::UIDebug()
	:UIWindow("Debug")
{
	m_Engine = &Engine::GetInstance();

	Camera * cam = m_Engine->GetWorld()->GetCurrentCamera();
	m_CameraPos = &cam->m_Position;
}

UIDebug::~UIDebug()
{
}

void UIDebug::DrawWindow()
{
	float camPos[3];

	camPos[0] = m_CameraPos->x;
	camPos[1] = m_CameraPos->y;
	camPos[2] = m_CameraPos->z;

	// draw the window
	ImGui::InputFloat3("Camera Position", camPos, 2);
	ImGui::Text("FPS = %u [Frame Time : %.2f]", m_Engine->GetFramePerSecond(), m_Engine->GetFrameTime() * 1'000);
}
