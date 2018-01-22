// Engine definition
// This contain a game scene, an update and manage different modules as the rendering engine...

#pragma once

#include <Windows.h>

// class predef
class World;
class Clock;

class DX12RenderEngine;

class Engine
{
public:
	struct EngineDesc
	{
		HINSTANCE	* HInstance = nullptr;
	};

	// singleton management
	static Engine &				GetInstance();
	static void					Create();
	static void					Delete();

	void		Initialize(EngineDesc & i_Desc);
	void		Run();

private:
	Engine();
	~Engine();

	// singleton
	static Engine *		s_Instance;

	// internal call
	void	CleanUpResources();
	void	CleanUpModules();

	// engine management
	bool			m_Exit;

	// DX12 rendering
	DX12RenderEngine *		m_RenderEngine;

	// game management
	World *			m_CurrentWorld;
	Clock *			m_EngineClock;

};