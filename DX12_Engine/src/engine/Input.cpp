#include "Input.h"

#include "engine/Debug.h"

void Input::BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const KeyEvent & i_Event, void * i_Data)
{
	UINT keyIndex = GetKeyIndex(i_KeyCode);

	if (keyIndex >= KEY_INDEX_MAX)
	{
		// this is not handled for now
		PRINT_DEBUG("Failed to bind event, %i key is not handled", i_KeyCode);
		return;
	}

	switch (i_Status)
	{
	case eKeyDown:

		break;

	case eKeyUp:

		break;
	}
}

LRESULT Input::ProcessInputCallbacks(HWND i_hWnd, UINT i_Param, WPARAM i_wParam, LPARAM i_lParam)
{
	switch (i_Param)
	{
		// keydown callback
	case WM_KEYDOWN:
		// keyup callback
		UINT key = GetKeyIndex(i_wParam);

		if (key < KEY_INDEX_MAX)
		{
			for (size_t i = 0; i < m_DownKeyEvents[key].size(); ++i)
		}


		break;
	case WM_KEYUP:
	}

	return true;
}

UINT Input::GetKeyIndex(UINT i_KeyIndex)
{
	// A to Z => 0 to 26
	if (i_KeyIndex >= 'A' && i_KeyIndex <= 'Z')
		return i_KeyIndex - 'A';	
	// F1 to F12 => 27 to 39
	if (i_KeyIndex >= VK_F1 && i_KeyIndex <= VK_F12)
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
