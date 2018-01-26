// Engine definition
// This contain a game scene, an update and manage different modules as the rendering engine...

#pragma once

#include <vector>
#include <Windows.h>
#include <DirectXMath.h>
#include "Window.h"
#include "dx12/d3dx12.h"
#include "../resource.h"

// precompilation define
#define WITH_EDITOR			1	// editor is binded in the executable

using namespace DirectX;

// class predef
// game
class World;
// engine
class Clock;
class RenderList;
class ResourcesManager;
// dx12
class DX12RenderEngine;
// ui
class UILayer;	// layer for UI

// class def
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
		// ui setup
		bool UIEnabled				= true;
	};

	// singleton management
	static Engine &				GetInstance();
	static void					Create();
	static void					Delete();

	// informations
	float		GetLifeTime() const;
	UINT		GetFramePerSecond() const;

	// intialize and run the engine
	void		Initialize(EngineDesc & i_Desc);
	void		Run();

	// window
	Window *		GetWindow() const;

	// accessors
	ResourcesManager *	GetResourcesManager() const;
	RenderList *		GetRenderList() const;
	World *				GetWorld() const;
	UILayer *			GetUILayer() const;

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
	UINT			m_FramePerSecond;

	// DX12 rendering
	DX12RenderEngine *		m_RenderEngine;
	RenderList *			m_RenderList;	// render list to render components

	// game management
	World *				m_CurrentWorld;
	Clock *				m_EngineClock;

	// ui
	UILayer *			m_UILayer;

	// managers
	ResourcesManager *	m_ResourcesManager;
};