#include "UISceneBuilder.h"

#include "editor/UIActorBuilder.h"

#include "ui/UI.h"
#include "game/World.h"
#include "game/Actor.h"
#include "game/ActorComponent.h"
#include "engine/Transform.h"

// static definition
const char *		UISceneBuilder::s_ActorSpawnType[] = { "Empty", "Cube", "Plane", "Light"};
Actor::ActorDesc *	UISceneBuilder::s_ActorDesc = new Actor::ActorDesc[_countof(s_ActorSpawnType)];

UISceneBuilder::UISceneBuilder(UIActorBuilder * i_ActorBuilder)
	:UIWindow("Scene Editor", eNone)
	,m_ActorBuilder(i_ActorBuilder)
	,m_SelectedActor(nullptr)
	,m_ActorToSetup(nullptr)
{
	// empty actor
	s_ActorDesc[0].Name = L"Empty";

	// cube actor
	s_ActorDesc[1].Name = L"Cube";
	s_ActorDesc[1].Mesh = L"Cube";

	// plane actor
	s_ActorDesc[2].Name = L"Plane";
	s_ActorDesc[2].Mesh = L"Plane";

	// light actor
	s_ActorDesc[3].Name = L"Light_Empty";	// To do
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

Actor * UISceneBuilder::GetSelectedActor() const
{
	return m_SelectedActor;
}

void UISceneBuilder::AddActor(const Actor::ActorDesc & i_Desc, const Transform & i_Transform)
{
	if (m_World == nullptr)	return;

	// spawn a new actor
	m_World->SpawnActor(i_Desc, i_Transform);
}

void UISceneBuilder::DeleteActor(Actor * i_Actor, bool i_DeleteChild)
{
	if (m_World == nullptr)	return;

	if (m_ActorBuilder->GetActor() == i_Actor)	m_ActorBuilder->SetActor(nullptr);
	m_World->DeleteActor(i_Actor, i_DeleteChild);
}

void UISceneBuilder::AttachToParent(Actor * i_Child, Actor * i_Parent)
{
	if (m_World == nullptr)	return;

	m_World->AttachActor(i_Parent, i_Child);
}

FORCEINLINE void UISceneBuilder::SelectActor(Actor * i_Actor)
{
	m_SelectedActor = i_Actor;

	if (m_ActorBuilder != nullptr)
	{
		m_ActorBuilder->SetActor(i_Actor);
	}
}

FORCEINLINE void UISceneBuilder::DrawActor(Actor * i_Actor)
{
	std::string actorName, actorId = String::IntToString(i_Actor->GetId());
	String::Utf16ToUtf8(actorName, i_Actor->GetName());
	

	// select the actor needed

	bool isOpen = ImGui::TreeNode(actorId.c_str(),actorName.c_str());

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			if (m_SelectedActor != i_Actor)
			{
				SelectActor(i_Actor);
			}
		}
		else if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("setup_actor");
			m_ActorToSetup = i_Actor;	// actor to setup
		}
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

	// draw differents actors here
	for (UINT i = 0; i < m_World->GetRootActorCount(); ++i)
	{
		Actor * actor = m_World->GetRootActorByIndex(i);
		DrawActor(actor);
	}

	// popup and other messages management
	// setup actor
	if (ImGui::BeginPopup("setup_actor") && m_ActorToSetup != nullptr)
	{
		ImGui::Text("Setup");
		ImGui::Separator();
		if (ImGui::Selectable("Remove"))
		{
			DeleteActor(m_ActorToSetup, false);
		}
		if (ImGui::Selectable("Remove All"))
		{
			DeleteActor(m_ActorToSetup, true);
		}
		ImGui::EndPopup();
	}
	else if (IsFocused())
	{
		if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("add_actor");
		}
	}

	// add actor to the scene
	if (ImGui::BeginPopup("add_actor"))
	{
		ImGui::Text("Add Actor");
		ImGui::Separator();
		for (int i = 0; i < _countof(s_ActorSpawnType); ++i)
		{
			if (ImGui::Selectable(s_ActorSpawnType[i]))
			{
				AddActor(s_ActorDesc[i], Transform());
			}
		}
		ImGui::EndPopup();
	}
	
}