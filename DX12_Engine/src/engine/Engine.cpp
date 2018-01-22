#include "Engine.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"
#include "game/World.h"
#include "engine/Clock.h"

Engine *		Engine::s_Instance = nullptr;

Engine & Engine::GetInstance()
{
	ASSERT(s_Instance != nullptr);
	return *s_Instance;
}

void Engine::Create()
{
	ASSERT(s_Instance == nullptr);
	s_Instance = new Engine;
}

void Engine::Delete()
{
	ASSERT(s_Instance != nullptr);
	delete s_Instance;
	s_Instance = nullptr;
}

void Engine::Initialize(EngineDesc & i_Desc)
{
	// create the DX12RenderEngine
	if (i_Desc.HInstance == nullptr)
	{
		PRINT_DEBUG("Error, hInstance not filled in Engine Desc");
		return;
	}

	// retreive the render engine
	DX12RenderEngine::Create(*i_Desc.HInstance);
	m_RenderEngine = &DX12RenderEngine::GetInstance();

	// create game
	m_EngineClock = new Clock;
	m_CurrentWorld = new World;

	m_Exit = false;
}

void Engine::Run()
{
	while (!m_Exit)
	{
		ASSERT(m_CurrentWorld != nullptr);
		m_CurrentWorld->TickWorld(0.f);


	}

	// exit the engine
	CleanUpResources();
	CleanUpModules();
}

Engine::Engine()
	:m_RenderEngine(nullptr)
	,m_CurrentWorld(nullptr)
	,m_EngineClock(nullptr)
	// setup
	,m_Exit(true)
{
}

Engine::~Engine()
{
}

void Engine::CleanUpResources()
{
}

void Engine::CleanUpModules()
{
	// delete the render engine
	m_RenderEngine->Delete();
}
