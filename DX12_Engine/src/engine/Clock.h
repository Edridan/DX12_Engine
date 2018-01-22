#pragma once

#include <Windows.h>

struct Time
{
	Time() {}
	Time(UINT64 i_Time)
		:m_Microsecs(i_Time)
	{}

	// getters
	float ToSeconds() const { return (m_Microsecs / 1'000'000.f); }

	// operators
	Time		operator+(const Time & i_Other) { return Time(m_Microsecs + i_Other.m_Microsecs); }
	Time		operator-(const Time & i_Other) { return Time(m_Microsecs - i_Other.m_Microsecs); }
	Time &		operator=(const Time & i_Other) { m_Microsecs = i_Other.m_Microsecs; return *this; }

	// time in microsecs
	UINT64		m_Microsecs;
};

class Clock
{
public:
	Clock();
	~Clock();

	// Informations related to the clock
	Time	GetElaspedTime() const;
	Time	Restart();
	void	Reset();

	Time	GetElapsedFromStart() const;

	// Informations from the system
	static Time	GetSystemTime();

private:
	// Time management
	Time	m_StartTime;
	Time	m_DefaultTime;	// time at the beginning
};