// console UI window

#pragma once

#include "UIWindow.h"

#define	HISTORY_BUFFER_SIZE		1024 * 1024

class UIConsole : public UIWindow
{
public:
	UIConsole();
	~UIConsole();

	// manage console
	void Print(const char * i_Text);
	void Clear();

	// callbacks
	static void StaticPrint(const char * i_Text, void * i_Data);

private:
	std::string		m_History;

	// containers
	char *		m_CommandBuffer;
	const int	m_BufferSize;


	// Inherited via UIWindow
	virtual void DrawWindow() override;
	// buffer text for rendering on the window
	void		PushCommandOnConsole();
};