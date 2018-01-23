#include "Engine.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"

#include "game/World.h"
#include "game/Camera.h"
#include "game/RenderComponent.h"

#include "engine/Clock.h"
#include "engine/Window.h"
#include "engine/ResourcesManager.h"
#include "engine/RenderList.h"

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

float Engine::GetLifeTime() const
{
	return m_EngineClock->GetElapsedFromStart().ToSeconds();
}

UINT Engine::GetFramePerSecond() const
{
	return m_FramePerSecond;
}

void Engine::Initialize(EngineDesc & i_Desc)
{
	// create the DX12RenderEngine
	if (i_Desc.HInstance == nullptr)
	{
		PRINT_DEBUG("Error, hInstance not filled in Engine Desc");
		return;
	}

	// create the window
	m_Window = new Window(i_Desc.HInstance, i_Desc.WindowName.c_str(), i_Desc.WindowName.c_str(), i_Desc.WindowSize.x, i_Desc.WindowSize.y, i_Desc.WindowIcon);

	// retreive the render engine
	DX12RenderEngine::Create(i_Desc.HInstance);
	m_RenderEngine = &DX12RenderEngine::GetInstance();
	m_RenderEngine->InitializeDX12();

	World::WorldDesc worldDesc;
	// create default camera parameters
	worldDesc.CameraPosition		= i_Desc.CameraPosition;
	worldDesc.CameraTarget			= i_Desc.CameraTarget;
	worldDesc.UseCameraProjection	= i_Desc.UseCameraProjection;
	worldDesc.CameraProjection		= i_Desc.CameraProjection;

	// create game
	m_EngineClock = new Clock;
	m_CurrentWorld = new World(worldDesc);

	// create managers
	m_RenderList = new RenderList;
	m_ResourcesManager = new ResourcesManager;

	// setup settings
	m_FramePerSecondsTargeted = i_Desc.FramePerSecondTargeted;

	m_Exit = false;
}

void Engine::Run()
{
	m_EngineClock->Reset();

	// To do : make a command list and a push to GPU
	// Workaround : render before doing anything
	m_RenderEngine->Render();

	while (!m_Exit)
	{
		// pre update management
		float elapsed = m_EngineClock->Restart().ToSeconds();

		// retreive performance data
		if (elapsed != 0.f)
		{
			m_FramePerSecond = 1 / elapsed;
		}

		/* -- Update -- */

		// update input and window callbacks
		m_Window->Update();

		// tick the world (update all actors and components)
		ASSERT(m_CurrentWorld != nullptr);
		m_CurrentWorld->TickWorld(elapsed);

		/* -- Render -- */

		// prepare the render engine
		m_RenderEngine->PrepareForRender();	// intialize the command list and other stuff

		// setup and push render list on the commandlist
		{
			RenderList::RenderListSetup setup;
			Camera * cam = m_CurrentWorld->GetCurrentCamera();

			// dx12 related
			setup.CommandList = m_RenderEngine->GetCommandList();
			// camera related
			setup.ProjectionMatrix	= XMLoadFloat4x4(&cam->GetProjMatrix());
			setup.ViewMatrix		= XMLoadFloat4x4(&cam->GetViewMatrix());

			// setup render list
			m_RenderList->Reset();	// reset the render list of the previous frame
			m_RenderList->SetupRenderList(setup);

			// render world
			m_CurrentWorld->RenderWorld(m_RenderList);

			// push the components on the commandlist to prepare for a render
			m_RenderList->PushOnCommandList();
		}

		// update and display backbuffer, also swap buffer and manage commandqueue
		m_RenderEngine->Render();

		/* -- End of the loop -- */
		// update exit 
		if (!m_Window->IsOpen())
		{
			m_Exit = true;
		}

		// wait before the next loop if we are too fast
		if (m_FramePerSecondsTargeted != 0)
		{
			// sleep time
			float frameTime = m_EngineClock->GetElaspedTime().ToSeconds();
			const float fpsTime = (1.f / (float)m_FramePerSecondsTargeted);

			if (frameTime < fpsTime)
			{
				DWORD sleepTime = (DWORD)((fpsTime - frameTime) * 1000.f);
				Sleep(sleepTime);
			}
		}
	}

	// exit the engine
	CleanUpResources();
	CleanUpModules();
}

Window * Engine::GetWindow() const
{
	return m_Window;
}

ResourcesManager * Engine::GetResourcesManager() const
{
	return m_ResourcesManager;
}

World * Engine::GetWorld() const
{
	return m_CurrentWorld;
}

RenderList * Engine::GetRenderList() const
{
	return m_RenderList;
}

Engine::Engine()
	:m_RenderEngine(nullptr)
	,m_CurrentWorld(nullptr)
	,m_EngineClock(nullptr)
	,m_Window(nullptr)
	// managers
	,m_ResourcesManager(nullptr)
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
	DX12RenderEngine::Delete();
}