// utils for debugging

#pragma once

#include <Windows.h>

#define ENABLE_DEBUG_BREAK 1

#if (ENABLE_DEBUG_BREAK)
#define DEBUG_BREAK		__debugbreak()
#else
#define DEBUG_BREAK		void(0)
#endif


// debug management
enum PopUpIcon
{
	eError		= MB_ICONERROR,
	eWarning	= MB_ICONWARNING,
};

#ifdef _DEBUG
void OutputDebug(const char * i_Text, ...);
void PopUpWindow(PopUpIcon i_Icon, const char * i_Notif, const char * i_Text, ...);

#define PRINT_DEBUG(i_Text, ...)	OutputDebug(i_Text, __VA_ARGS__)
#define POPUP_WARNING(i_Text, ...)	PopUpWindow(eWarning, "Warning", i_Text, __VA_ARGS__)
#define POPUP_ERROR(i_Text, ...)	PopUpWindow(eError, "Error", i_Text, __VA_ARGS__)

// Assert e
#define ASSERT(i_Condition)													\
do																			\
{																			\
	if (!(i_Condition))														\
	{																		\
		PRINT_DEBUG("%s assert an error file : %s, [%i]", #i_Condition,		\
				__FILE__, __LINE__);										\
		PopUpWindow(eError, "Assert", " %s Assert error file : %s [%i]",	\
				#i_Condition, __FILE__, __LINE__);							\
		DEBUG_BREAK;														\
	}																		\
} while (false)

#else

#define PRINT_DEBUG(i_Text, ...)	
#define PRINT_DEBUG(i_Text, ...)	OutputDebug(i_Text, ...)
#define POPUP_WARNING(i_Text, ...)	PopUpWindow(eWarning, "Warning", i_Text, ...)

#define DX12_ASSERT(i_Hres)		i_Hres
#define ASSERT(i_Condition)		i_Condition

#endif