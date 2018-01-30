#include "Debug.h"

#if PRINT_DEBUG_ON_CONSOLE
#include "engine/Engine.h"
#include "engine/Console.h"
#endif

// vsnprintf, sprintf_s
#include <cstdarg>
#include <stdio.h>

// size of the buffer for print debug
#define BUFFER_SIZE			2048

// debug functions
#ifdef _DEBUG

void OutputDebug(const char * i_Text, ...)
{
	static char buffer[BUFFER_SIZE];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	vsnprintf(buffer, BUFFER_SIZE, i_Text, args);
	va_end(args);

#if PRINT_DEBUG_ON_CONSOLE
	Console * console = Engine::GetInstance().GetConsole();
	if (console)	
		console->Print(buffer);
#endif
	OutputDebugStringA(buffer);
}

void OutputDebugVS(const char * i_Text, ...)
{
	static char buffer[BUFFER_SIZE];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	vsnprintf(buffer, BUFFER_SIZE, i_Text, args);
	va_end(args);

	OutputDebugStringA(buffer);
}

void PopUpWindow(PopUpIcon i_Icon, const char * i_Notif, const char * i_Text, ...)
{
	static char buffer[BUFFER_SIZE];
	const char * p = buffer;

	va_list args;
	va_start(args, i_Text);
	sprintf_s(buffer, i_Text, args);
	va_end(args);

	MessageBoxA(NULL, buffer,
		i_Notif, MB_OK | MB_ICONERROR);
}

#endif