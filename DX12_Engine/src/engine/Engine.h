// Engine definition
// This contain a game scene, an update and manage different modules as the rendering engine...

#pragma once

#include <vector>
#include <Windows.h>
#include <DirectXMath.h>
#include "Window.h"
#include "dx12/d3dx12.h"
#include "../resource.h"


using namespace DirectX;

// class predef
class World;
class Clock;
class ResourcesManager;
class DX12RenderEngine;

// class defined in this file
class RenderList;

class Engine
{
public:
	struct EngineDesc
	{
		// engine setup
		HINSTANCE	HInstance				= nullptr;
		UINT		FramePerSecondTargeted	= 60;
		// window setup
		IntVec2 WindowSize			= IntVec2(1600, 900);
		std::wstring WindowName		= L"DX12_Engine";
		Window::Icon WindowIcon		= Window::Icon((LPWSTR)(IDI_ICON1), 32, 32);
		// camera setup
		XMFLOAT4 CameraPosition		= XMFLOAT4(0.f, 0.f, -1.f, 1.f);
		XMFLOAT4 CameraTarget		= XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		bool UseCameraProjection	= false;
		XMMATRIX CameraProjection	= XMMatrixIdentity();
	};

	// singleton management
	static Engine &				GetInstance();
	static void					Create();
	static void					Delete();

	// informations
	float		GetLifeTime() const;

	// intialize and run the engine
	void		Initialize(EngineDesc & i_Desc);
	void		Run();

	// window
	Window *		GetWindow() const;

	// accessors
	ResourcesManager *	GetResourcesManager() const;
	RenderList *		GetRenderList() const;
	World *				GetWorld() const;

private:
	Engine();
	~Engine();

	// singleton
	static Engine *		s_Instance;

	// internal call
	void	CleanUpResources();
	void	CleanUpModules();

	// window
	Window *		m_Window;

	// engine management
	bool			m_Exit;
	UINT			m_FramePerSecondsTargeted;

	// DX12 rendering
	DX12RenderEngine *		m_RenderEngine;

	// game management
	World *				m_CurrentWorld;
	Clock *				m_EngineClock;

	// managers
	ResourcesManager *	m_ResourcesManager;
	RenderList *		m_RenderList;
};

// class predef
class RenderComponent;

// this class is used by the engine to create and manage correclty components to draw for one frame
// To do : clean this part of dirty code
class RenderList
{
public:
	RenderList(World * i_World);
	~RenderList();

	// push a render component to add to rendering pipeline
	void PushRenderComponent(RenderComponent * i_RenderComponent);

	// push the render list on the command list
	void Render(ID3D12GraphicsCommandList * i_CommandList);

private:
	std::vector<RenderComponent*>	m_Components;
	World *							m_World;
};