#include "UIActorBuilder.h"

#include "ui/UI.h"

#include "game/Actor.h"

UIActorBuilder::UIActorBuilder()
	:UIWindow("Actor Builder", eNone)
	,m_Actor(nullptr)
{
}

UIActorBuilder::~UIActorBuilder()
{
}

void UIActorBuilder::SetActor(Actor * i_Actor)
{
	m_Actor = i_Actor;
}

FORCEINLINE void UIActorBuilder::DrawTransform(Transform * i_Transform)
{
	XMFLOAT3 saved;
	DrawVector(i_Transform->GetPosition(), "Position", &saved);
	if (!Float3Eq(i_Transform->GetPosition(), saved))	i_Transform->SetPosition(saved);
	DrawVector(i_Transform->GetRotation(), "Rotation", &saved);
	if (!Float3Eq(i_Transform->GetRotation(), saved))	i_Transform->SetRotation(saved);
	DrawVector(i_Transform->GetScale(), "Scale   ", &saved);
	if (!Float3Eq(i_Transform->GetScale(), saved))		i_Transform->SetScale(saved);

}

FORCEINLINE void UIActorBuilder::DrawVector(DirectX::XMFLOAT3 & i_Vector, const char * i_Name, DirectX::XMFLOAT3 * o_Save)
{
	float transPos[3];

	transPos[0] = i_Vector.x;
	transPos[1] = i_Vector.y;
	transPos[2] = i_Vector.z;

	// draw the window
	ImGui::InputFloat3(i_Name, transPos, 2);

	// output saved vector
	if (o_Save != nullptr)
	{
		o_Save->x = transPos[0];
		o_Save->y = transPos[1];
		o_Save->z = transPos[2];
	}
}

void UIActorBuilder::DrawWindow()
{
	if (m_Actor == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No actor selected from Scene Builder");
		return;
	}

	// draw name of the actor
	static char name[128];

	std::string actorName;
	std::wstring WActorName = m_Actor->GetName(), WNewActorName;

	String::Utf16ToUtf8(actorName, WActorName);
	strcpy_s(name, actorName.c_str());
	ImGui::Text("Actor Name");
	ImGui::SameLine();
	ImGui::InputText("", name, 128);
	String::Utf8ToUtf16(WNewActorName, name);
	m_Actor->SetName(WNewActorName);

	// draw the transform of the object
	ImGui::Separator();
	Transform * trans = &m_Actor->m_Transform;
	DrawTransform(trans);

	ImGui::Separator();


}

