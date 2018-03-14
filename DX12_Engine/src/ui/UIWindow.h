// virtual class to manage a UI window
// to create a window, simply inherit from this UIWindow and implement rendering function

#pragma once

#include <string>

class UIWindow
{
public:
	enum UIWindowFlags
	{
		// none flags available
		eNone					= 0,
		// no title bar
		eNoTitleBar				= 1 << 0,	// Disable title bar for the window
		eNoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
		eNoMove                 = 1 << 2,   // Disable user moving the window
		eNoScrollbar            = 1 << 3,   // Disable scrollbar (window can still scroll with mouse or programatically)
		eNoScrollWithMouse      = 1 << 4,   // Disable user scrolling with mouse wheel
		eNoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it
		eAlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
		eShowBorders            = 1 << 7,   // Show borders around windows and items
		eNoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
		eNoInputs               = 1 << 9,   // Disable catching mouse or keyboard inputs
		eMenuBar                = 1 << 10,  // Has a menu-bar
	};

	UIWindow(const std::string & i_Name, int i_Flags = UIWindowFlags::eNone);
	virtual ~UIWindow();

	// information for UILayer
	bool			IsActive() const;
	void			SetActive(bool i_Active);
	unsigned int	GetId() const;
	const char *	GetName() const;
	bool			IsFocused() const;
	bool			IsHovered() const;

	// Draw window using imgui, you can update the data here
	void			StartDraw();
	virtual void	DrawWindow() = 0;
	void			EndDraw() const;

	// friend class
	friend class UILayer;
private:
	// internal overrited callbacks
	virtual void		OnClose(bool i_UserCall);
	virtual void		OnOpen();
	virtual void		OnHover();
	virtual void		OnStartHover();
	virtual void		OnEndHover();
	virtual void		OnCollapsed();

	// window management
	bool			m_Active;
	bool			m_Focused;
	unsigned int	m_Id;
	int				m_WindowFlags;
	std::string		m_WindowName;

	// window callbacks management
	bool			m_IsHovered;
	bool			m_IsCollapsed;

	// static
	static unsigned int s_Instance;
};
