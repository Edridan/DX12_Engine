#include "Input.h"

#include "engine/Debug.h"

// static definition
std::vector<Input::KeyEventStruct>		Input::m_DownKeyEvents[KEY_INDEX_MAX];
std::vector<Input::KeyEventStruct>		Input::m_UpKeyEvents[KEY_INDEX_MAX];


void Input::BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode,const std::string & i_NameId, const KeyEvent & i_Event, void * i_Data)
{
	UINT keyIndex = GetKeyIndex(i_KeyCode);

	if (keyIndex >= KEY_INDEX_MAX)
	{
		// this is not handled for now
		PRINT_DEBUG("Failed to bind event, %i key is not handled", i_KeyCode);
		return;
	}

	// struct to manage event
	KeyEventStruct keyEvent;

	keyEvent.Callback	= i_Event;
	keyEvent.UserData	= i_Data;
	keyEvent.NameId		= i_NameId;
	keyEvent.KeySetup	= EKeySetupFlags::eNone;

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

void Input::BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, UINT i_KeySetupFlag,const std::string & i_NameId, const KeyEvent & i_Event, void * i_Data)
{
	UINT keyIndex = GetKeyIndex(i_KeyCode);

	if (keyIndex >= KEY_INDEX_MAX)
	{
		// this is not handled for now
		PRINT_DEBUG("Failed to bind event, %i key is not handled", i_KeyCode);
		return;
	}

	// struct to manage event
	KeyEventStruct keyEvent;

	keyEvent.Callback = i_Event;
	keyEvent.UserData = i_Data;
	keyEvent.NameId = i_NameId;
	keyEvent.KeySetup = i_KeySetupFlag;

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

void Input::UnbindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const std::string & i_NameId)
{
	UINT key = GetKeyIndex(i_KeyCode);

	if (i_Status == eKeyDown)
	{
		auto itr = m_DownKeyEvents[key].begin();

		while (itr != m_DownKeyEvents[key].end())
		{
			if ((*itr).NameId == i_NameId)
			{
				m_DownKeyEvents[key].erase(itr);
				return;
			}
			++itr;
		}
	}
	else if (i_Status == eKeyUp)
	{
		auto itr = m_UpKeyEvents[key].begin();

		while (itr != m_UpKeyEvents[key].end())
		{
			if ((*itr).NameId == i_NameId)
			{
				m_UpKeyEvents[key].erase(itr);
				return;
			}
			++itr;
		}
	}

	// didn't find the callback to unbind
	PRINT_DEBUG("Unable to unbind callback : %i (not found)", i_NameId.c_str());
}

LRESULT Input::ProcessInputCallbacks(HWND i_hWnd, UINT i_Param, WPARAM i_wParam, LPARAM i_lParam)
{
	// keyup callback
	UINT key = GetKeyIndex(i_wParam);
	UINT keyState = GetKeySetupFlags();

	switch (i_Param)
	{
		// keydown callback
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (key < KEY_INDEX_MAX)
		{
			auto itr = m_DownKeyEvents[key].begin();

			// callbacks
			while (itr != m_DownKeyEvents[key].end())
			{
				if ((*itr).KeySetup == keyState || ((*itr).KeySetup == eNone))
				{
					((*itr).Callback)((*itr).UserData);
				}
				++itr;
			}
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// keyup callback
		if (key < KEY_INDEX_MAX)
		{
			auto itr = m_UpKeyEvents[key].begin();

			// callbacks
			while (itr != m_UpKeyEvents[key].end())
			{
				if ((*itr).KeySetup == keyState || ((*itr).KeySetup == eNone))
				{
					((*itr).Callback)((*itr).UserData);
				}

				++itr;
			}
		}
	}

	return LRESULT(true);
}

#pragma warning(disable:4244)

UINT Input::GetKeyIndex(WPARAM i_KeyIndex)
{
	// A to Z => 0 to 26
	if (i_KeyIndex >= 'A' && i_KeyIndex <= 'Z')
		return i_KeyIndex - 'A';	
	// F1 to F12 => 27 to 39
	if (i_KeyIndex >= VK_F1 && i_KeyIndex <= VK_F11)	// To do : F12 is broken?
		return (i_KeyIndex - VK_F1) + 27;
	// Numpad => 40 to 49
	if (i_KeyIndex >= VK_NUMPAD0 && i_KeyIndex >= VK_NUMPAD9)
		return (i_KeyIndex - VK_NUMPAD0) + 40;
	// special char => 50 to 64
	switch (i_KeyIndex)
	{
	case VK_SPACE:		return 50;
	case VK_CANCEL:		return 51;
	case VK_SHIFT:		return 52;
	case VK_ESCAPE:		return 53;
	case VK_LEFT:		return 54;
	case VK_UP:			return 55;
	case VK_DOWN:		return 56;
	case VK_RIGHT:		return 57;
	case VK_DELETE:		return 58;
	case VK_SUBTRACT:	return 59;
	case VK_DIVIDE:		return 60;
	case VK_NUMLOCK:	return 61;
	case VK_LSHIFT:		return 62;
	case VK_RSHIFT:		return 63;
	}

	return KEY_INDEX_MAX;	// none found
}

#pragma warning(default:4244)

UINT Input::GetKeySetupFlags()
{
	UINT flags = EKeySetupFlags::eNone;

	// retreive state for the keys
	if (GetAsyncKeyState(VK_SHIFT))		
		flags |= EKeySetupFlags::eShiftDown;
	if (GetAsyncKeyState(VK_MENU))		
		flags |= EKeySetupFlags::eAltDown;
	if (GetAsyncKeyState(VK_CONTROL))	
		flags |= EKeySetupFlags::eCtrlDown;

	return flags;
}
