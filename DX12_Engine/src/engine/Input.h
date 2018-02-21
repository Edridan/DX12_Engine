// input management callbacks for the engine
// this can handle callbacks on specific input
// this is a static class
#pragma once

#include <Windows.h>
#include <vector>
#include <functional>
#include "engine/Debug.h"

class Input
{
public:
	enum EKeyEvent
	{
		eKeyDown,
		eKeyUp,
	};

	enum EKeySetupFlags
	{
		// no need other inputs
		eNone		= 0,		
		// need some other inputs
		eAltDown	= 1 << 0,
		eCtrlDown	= 1 << 1,
		eShiftDown	= 1 << 2,
	};

	// struct for helping
	struct ShortCut
	{
		EKeySetupFlags	KeyFlags;
		UINT			KeyCode;

		// default constructor
		ShortCut(UINT i_Key = 0, EKeySetupFlags i_Flags = eNone)
			:KeyFlags(i_Flags)
			,KeyCode(i_Key)
		{};
	};

#define KEY_INDEX_MAX		64

	// define events
	typedef  std::function<void(void *)>	KeyEvent;

	// register callbacks
	template <class _Type>
	static void BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId, _Type * const i_Ptr, void(_Type::*const i_Func)(void*), void * i_Data);
	template <class _Type>
	static void BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId,  UINT i_KeyFlags, _Type * const i_Ptr, void(_Type::*const i_Func)(void*), void * i_Data);

	// call for static
	static void	BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId, const KeyEvent & i_Event, void * i_Data);
	static void	BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, UINT i_KeySetupFlag, const std::string & i_NameId, const KeyEvent & i_Event, void * i_Data);
	static void UnbindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId);

	// process all callbacks
	static LRESULT ProcessInputCallbacks(HWND i_hWnd, UINT i_Param, WPARAM i_wParam, LPARAM i_lParam);
private:
	// convert key from index
	static UINT		GetKeyIndex(WPARAM i_KeyIndex);
	static UINT		GetKeySetupFlags();

	struct KeyEventStruct
	{
		KeyEvent		Callback;
		void *			UserData;
		UINT			KeySetup;
		std::string		NameId;	// To do : register unique events to unbind them if necessary
	};

	// input callback management
	static std::vector<KeyEventStruct>		m_DownKeyEvents[KEY_INDEX_MAX];
	static std::vector<KeyEventStruct>		m_UpKeyEvents[KEY_INDEX_MAX];
};

template<class _Type>
inline void Input::BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId, _Type * const i_Ptr, void(_Type::* const i_Func)(void *), void * i_Data)
{
	using namespace std::placeholders;

	UINT keyIndex = GetKeyIndex(i_KeyCode);

	if (keyIndex >= KEY_INDEX_MAX)
	{
		// this is not handled for now
		PRINT_DEBUG("Failed to bind event, %i key is not handled", i_KeyCode);
		return;
	}

	// struct to manage event
	KeyEventStruct keyEvent;

	keyEvent.Callback = std::bind(i_Func, i_Ptr, _1);
	keyEvent.UserData = i_Data;
	keyEvent.NameId = i_NameId;
	keyEvent.KeySetup = EKeySetupFlags::eNone;

	switch (i_Status)
	{
	case eKeyDown:
		m_DownKeyEvents[keyIndex].push_back(keyEvent);
		break;

	case eKeyUp:
		m_UpKeyEvents[keyIndex].push_back(keyEvent);
		break;
	}
}

template<class _Type>
inline void Input::BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId, UINT i_KeyFlags, _Type * const i_Ptr, void(_Type::*const i_Func)(void*), void * i_Data)
{
	using namespace std::placeholders;

	UINT keyIndex = GetKeyIndex(i_KeyCode);

	if (keyIndex >= KEY_INDEX_MAX)
	{
		// this is not handled for now
		PRINT_DEBUG("Failed to bind event, %i key is not handled", i_KeyCode);
		return;
	}

	// struct to manage event
	KeyEventStruct keyEvent;

	keyEvent.Callback = std::bind(i_Func, i_Ptr, _1);
	keyEvent.UserData = i_Data;
	keyEvent.NameId = i_NameId;
	keyEvent.KeySetup = i_KeyFlags;

	switch (i_Status)
	{
	case eKeyDown:
		m_DownKeyEvents[keyIndex].push_back(keyEvent);
		break;

	case eKeyUp:
		m_UpKeyEvents[keyIndex].push_back(keyEvent);
		break;
	}
}
