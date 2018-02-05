// input management callbacks for the engine
// this can handle callbacks on specific input
// this is a static class
#pragma once

#include <Windows.h>
#include <vector>
#include <functional>

static class Input
{
public:
	enum EKeyEvent
	{
		eKeyDown,
		eKeyUp,
	};

#define KEY_INDEX_MAX		64

	// define events
	typedef  std::function<void()>	KeyEvent;

	// register callbacks
	void	BindKeyEvent(EKeyEvent i_Status, UINT i_KeyCode, const KeyEvent & i_Event, void * i_Data);

	// process all callbacks
	LRESULT ProcessInputCallbacks(HWND i_hWnd, UINT i_Param, WPARAM i_wParam, LPARAM i_lParam);
private:
	// convert key from index
	UINT		GetKeyIndex(UINT i_KeyIndex);

	// input callback management
	std::vector<std::pair<KeyEvent, void *>>		m_DownKeyEvents[KEY_INDEX_MAX];
	std::vector<std::pair<KeyEvent, void *>>		m_UpKeyEvents[KEY_INDEX_MAX];
};