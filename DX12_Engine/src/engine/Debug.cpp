#include "Debug.h"

// vsnprintf, sprintf_s
#include <cstdarg>
#include <stdio.h>

// debug functions
#ifdef _DEBUG

void OutputDebug(const char * i_Text, ...)
{
	static char buffer[2048];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	vsnprintf(buffer, 2048, i_Text, args);
	va_end(args);

	OutputDebugStringA(buffer);
}

void PopUpWindow(PopUpIcon i_Icon, const char * i_Notif, const char * i_Text, ...)
{
	static char buffer[2048];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	sprintf_s(buffer, i_Text, args);
	va_end(args);

	MessageBoxA(NULL, buffer,
		i_Notif, MB_OK | MB_ICONERROR);
}

#endif