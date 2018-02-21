// Editor manager
// manage the editor windows, inputs

#pragma once

#include "engine/Defines.h"

#ifdef WITH_EDITOR

// Editor differents windows
class UIMaterialBuilder;
class UISceneBuilder;
class UIActorBuilder;
class UILayer;

#include "engine/Input.h"

class Editor
{
public:
	friend class Engine;

	// manage
	void		SetActive(bool i_Active);
	bool		IsActive() const;

private:

	struct EditorDesc
	{
		UILayer *		Layer;
		bool			EnabledByDefault = false;
		// shortcuts
		Input::ShortCut		InputShowMaterial	= Input::ShortCut(0x4D);	// M
		Input::ShortCut		InputShowScene		= Input::ShortCut(0x57);	// W
		Input::ShortCut		InputActorBuilder	= Input::ShortCut(0x4C);	// L
	};

	struct EditorShortCuts
	{
		// shortcuts
		Input::ShortCut		InputShowMaterial	= Input::ShortCut(0x4D);
		Input::ShortCut		InputShowScene		= Input::ShortCut(0x57);
		Input::ShortCut		InputActorBuilder	= Input::ShortCut(0x4C);
	};

	Editor(const EditorDesc & i_Desc);
	~Editor();

	// internal
	void		BindKeyboardEvents();
	void		UnbindKeyboardEvents();
	void		CloseEditor();
	void		OpenEditor();

	// editor callback events
	void			SetMaterialBuilderShow(void * i_Void);
	void			SetSceneBuilderShow(void * i_Void);
	void			SetEditorShow(void * i_Void);

	// editor management
	UIMaterialBuilder *		m_MaterialBuilder;
	UISceneBuilder *		m_SceneBuilder;
	UIActorBuilder *		m_ActorBuilder;
	bool					m_IsEnabled;

	// engine
	EditorShortCuts			m_Shortcuts;
	UILayer *				m_Layer;
};



#endif