#include "Clock.h"

#include <windows.h>

Clock::Clock()
{
	m_StartTime = Clock::GetSystemTime();
	m_DefaultTime = m_StartTime;
}

Clock::~Clock()
{
}

Time Clock::GetElaspedTime() const
{
	// Get elapsed time from the last start
	return Clock::GetSystemTime() - m_StartTime;
}

Time Clock::Restart()
{
	// Get the elapsed time from the current time and the start time
	Time current = Clock::GetSystemTime();
	Time elapsed = current - m_StartTime;

	// Update the start time
	m_StartTime = current;

	return Time(elapsed);
}

void Clock::Reset()
{
	m_StartTime = Clock::GetSystemTime();
	m_DefaultTime = m_StartTime;
}

Time Clock::GetElapsedFromStart() const
{
	// Get elapsed time from the last start
	return Clock::GetSystemTime() - m_DefaultTime;
}

Time Clock::GetSystemTime()
{
	// Get Frequency of the system (static variable!)
	static LARGE_INTEGER frequency = {};
	QueryPerformanceFrequency(&frequency);

	// Now get the time of the system
	LARGE_INTEGER time = {};
	QueryPerformanceCounter(&time);

	return Time(1'000'000 * time.QuadPart / frequency.QuadPart);
}
