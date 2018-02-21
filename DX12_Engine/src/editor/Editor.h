// Editor manager
// manage the editor windows, inputs

#pragma once

#include "engine/Engine.h"
#include <WinUser.h>

#ifdef WITH_EDITOR

// Editor differents windows
class UIMaterialBuilder;
class UISceneBuilder;
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
		Input::ShortCut		InputShowMaterial	= Input::ShortCut(0x4D);
		Input::ShortCut		InputShowScene		= Input::ShortCut(0x57);
	};

	struct EditorShortCuts
	{
		// shortcuts
		Input::ShortCut		InputShowMaterial = Input::ShortCut(0x4D);
		Input::ShortCut		InputShowScene = Input::ShortCut(0x57);
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
	bool					m_IsEnabled;

	// engine
	EditorShortCuts			m_Shortcuts;
	UILayer *				m_Layer;
};



#endif