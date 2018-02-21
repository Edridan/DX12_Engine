#include "UISceneBuilder.h"

#include "ui/UI.h"
#include "game/World.h"
#include "game/Actor.h"
#include "game/ActorComponent.h"
#include "engine/Transform.h"

UISceneBuilder::UISceneBuilder()
	:UIWindow("Scene Editor", UIWindowFlags::eShowBorders)
	,m_World(nullptr)
{

}

UISceneBuilder::~UISceneBuilder()
{
}

void UISceneBuilder::SetWorld(World * i_World)
{
	m_World = i_World;

	if (m_World != nullptr)
	{
		// update vectors
		m_Actors.clear();
		m_RootActors.clear();

		for (UINT i = 0; i < m_World->GetRootActorCount(); ++i)
		{
			m_RootActors.push_back(m_World->GetRootActorByIndex(i));
		}
	}
}

World * UISceneBuilder::GetWorld() const
{
	return m_World;
}

void UISceneBuilder::DrawWindow()
{
	// error management
	if (m_World == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No world binded to the window");
		return;
	}

	

	if (ImGui::BeginMenu("Menu"))
	{
		std::string actorName;

		// for each actors
		auto itr = m_RootActors.begin();
		while (itr != m_RootActors.end())
		{
			String::Utf16ToUtf8(actorName, (*itr)->GetName());

			if (ImGui::TreeNode(actorName.c_str()))
			{
				// draw component
				DrawActorObject(*itr);

				// finish rendering the actor
				ImGui::TreePop();
			}

			++itr;
		}

		ImGui::EndMenu();
	}
	

	
}

FORCEINLINE void UISceneBuilder::DrawActorObject(Actor * i_Actor)
{
	Transform * trans = & i_Actor->m_Transform;

	// draw the transform of the object
	DrawTransform(trans);
}

FORCEINLINE void UISceneBuilder::DrawTransform(Transform * i_Transform)
{
	XMFLOAT3 saved;
	DrawVector(i_Transform->GetPosition(), "Position", &saved);
	if (!Float3Eq(i_Transform->GetPosition(), saved))	i_Transform->SetPosition(saved);


}

FORCEINLINE void UISceneBuilder::DrawVector(DirectX::XMFLOAT3 & i_Vector, const char * i_Name, DirectX::XMFLOAT3 * o_Save)
{
	float transPos[3];

	transPos[0] = i_Vector.x;
	transPos[1] = i_Vector.y;
	transPos[2] = i_Vector.z;

	// draw the window
	ImGui::InputFloat3("Location", transPos, 2);

	// output saved vector
	if (o_Save != nullptr)
	{
		o_Save->x = transPos[0];
		o_Save->y = transPos[1];
		o_Save->z = transPos[2];
	}
}
