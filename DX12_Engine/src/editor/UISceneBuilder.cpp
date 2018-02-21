#include "UISceneBuilder.h"

#include "editor/UIActorBuilder.h"

#include "ui/UI.h"
#include "game/World.h"
#include "game/Actor.h"
#include "game/ActorComponent.h"
#include "engine/Transform.h"

UISceneBuilder::UISceneBuilder(UIActorBuilder * i_ActorBuilder)
	:UIWindow("Scene Editor", eNone)
	,m_ActorBuilder(i_ActorBuilder)
{
}

UISceneBuilder::~UISceneBuilder()
{
}

void UISceneBuilder::SetWorld(World * i_World)
{
	m_World = i_World;
}

World * UISceneBuilder::GetWorld() const
{
	return m_World;
}

void UISceneBuilder::AddEmptyActor(const Transform & i_Transform, const char * i_Name)
{
}

FORCEINLINE void UISceneBuilder::DrawActor(Actor * i_Actor)
{
	std::string actorName;
	String::Utf16ToUtf8(actorName, i_Actor->GetName());

	// select the actor needed
	bool isOpen = ImGui::TreeNode(actorName.c_str());
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
	{
		m_ActorBuilder->SetActor(i_Actor);
	}

	// draw children if needed
	if (isOpen)
	{
		// render childs
		if (i_Actor->HaveChild())
		{
			for (UINT i = 0; i < i_Actor->ChildCount(); ++i)
			{
				DrawActor(i_Actor->GetChild(i));
			}
		}
		else
		{
			ImGui::Text("No child");
		}

		// finish rendering the actor
		ImGui::TreePop();
	}
}

void UISceneBuilder::DrawWindow()
{
	// error management
	if (m_World == nullptr)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No world binded to the window");
		return;
	}


	// for each root actors
	for (UINT i = 0; i < m_World->GetRootActorCount(); ++i)
	{
		Actor * actor = m_World->GetRootActorByIndex(i);
		DrawActor(actor);
	}
}