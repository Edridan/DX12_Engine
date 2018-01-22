#include "Engine.h"

#include "dx12/DX12Utils.h"
#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12Window.h"

#include "game/World.h"
#include "game/Camera.h"
#include "game/RenderComponent.h"

#include "engine/Clock.h"
#include "engine/ResourcesManager.h"

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

void Engine::Initialize(EngineDesc & i_Desc)
{
	// create the DX12RenderEngine
	if (i_Desc.HInstance == nullptr)
	{
		PRINT_DEBUG("Error, hInstance not filled in Engine Desc");
		return;
	}

	// retreive the render engine
	DX12RenderEngine::Create(i_Desc.HInstance);
	m_RenderEngine = &DX12RenderEngine::GetInstance();
	m_RenderEngine->InitializeDX12();

	World::WorldDesc worldDesc;
	// create default camera parameters
	worldDesc.CameraPosition		= i_Desc.DefaultCamera.CameraPosition;
	worldDesc.CameraTarget			= i_Desc.DefaultCamera.CameraTarget;
	worldDesc.UseCameraProjection	= i_Desc.DefaultCamera.UseCameraProjection;
	worldDesc.CameraProjection		= i_Desc.DefaultCamera.CameraProjection;

	// create game
	m_EngineClock = new Clock;
	m_CurrentWorld = new World(worldDesc);

	// create managers
	m_RenderList = new RenderList(m_CurrentWorld);
	m_ResourcesManager = new ResourcesManager;

	// setup settings
	m_FramePerSecondsTargeted = i_Desc.FramePerSecondTargeted;

	m_Exit = false;
}

void Engine::Run()
{
	m_EngineClock->Reset();

	DX12Window & window = m_RenderEngine->GetWindow();

	// To do : make a command list and a push to GPU
	// Workaround : render before doing anything
	m_RenderEngine->Render();

	while (!m_Exit)
	{
		float elapsed = m_EngineClock->Restart().ToSeconds();

		// input management and window update
		m_RenderEngine->UpdateWindow();

		// tick the world (update all actors and components)
		ASSERT(m_CurrentWorld != nullptr);
		m_CurrentWorld->TickWorld(elapsed);

		m_RenderEngine->PrepareForRender();

		m_RenderList->Render(m_RenderEngine->GetCommandList());
		
		m_RenderEngine->Render();

		// update exit 
		if (!window.IsOpen())
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


// render list implementation
RenderList::RenderList(World * i_World)
	:m_World(i_World)
{
}

RenderList::~RenderList()
{
}

void RenderList::PushRenderComponent(RenderComponent * i_RenderComponent)
{
	ASSERT(i_RenderComponent->GetActor()->GetWorld() == m_World);
	m_Components.push_back(i_RenderComponent);
}

void RenderList::Render(ID3D12GraphicsCommandList * i_CommandList)
{
	Camera * cam = m_World->GetCurrentCamera();
	DX12RenderEngine & render = DX12RenderEngine::GetInstance();

	// render the opaque geometry
	for (size_t i = 0; i < m_Components.size(); ++i)
	{
		// retreive and update the constant buffer
		ADDRESS_ID constBuffer	= m_Components[i]->GetConstBufferAddress();
		Actor *	actor			= m_Components[i]->GetActor();

		// update the model view transform matrix
		DX12RenderEngine::DefaultConstantBuffer constantBuffer;

		// error 
		if (constBuffer == UnavailableAdressId)
		{
			PRINT_DEBUG("Error, component doesn't have any const buffer mapped address");
			continue;
		}

		XMMATRIX viewMat = XMLoadFloat4x4(&cam->GetViewMatrix()); // load view matrix
		XMMATRIX projMat = XMLoadFloat4x4(&cam->GetProjMatrix()); // load projection matrix
		XMMATRIX modelMat = actor->GetWorldTransform(); // create mvp matrix

		// update data into the const buffer
		XMStoreFloat4x4(&constantBuffer.m_Model, XMMatrixTranspose(modelMat));
		XMStoreFloat4x4(&constantBuffer.m_View, XMMatrixTranspose(viewMat));
		XMStoreFloat4x4(&constantBuffer.m_Projection, XMMatrixTranspose(projMat));

		// other data
		constantBuffer.m_CameraForward = XMFLOAT3((const float*)&cam->GetFoward());

		// update the constant buffer
		render.UpdateConstantBuffer(constBuffer, constantBuffer);

		// push all needed data to the command list
		m_Components[i]->PushOnCommandList(i_CommandList);
	}

	m_Components.clear();
}
