// Code for DX12 sampler tutorial


// DX12
#include <DirectXMath.h>
#include "dx12/d3dx12.h"

#include "dx12/DX12RenderEngine.h"
#include "dx12/DX12Mesh.h"
#include "dx12/DX12Font.h"
#include "dx12/DX12Text.h"

// Game
#include "game/GameScene.h"
#include "game/GameObject.h"
#include "engine/Clock.h"

int WINAPI WinMain(
	HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	// Initializing rendering, window
	// Create the window and initialize dx12
	DX12RenderEngine::Create(hInstance);

	DX12RenderEngine & renderEngine = DX12RenderEngine::GetInstance();
	renderEngine.InitializeDX12();

	// Initialize game scene
	GameScene::GameSceneDesc gameSceneDesc;
	gameSceneDesc.CameraPosition	= DirectX::XMFLOAT4(0.0, 0.f, 5.f, 0.f);
	gameSceneDesc.CameraTarget		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	GameScene game(gameSceneDesc);

	// load data here
	DX12Mesh * mesh = DX12Mesh::LoadMeshObj("resources/obj/cube.obj");
	DX12Font font(L"resources/font/Arial-font.fnt");

	//DX12Text renderText(L"Text");

	// To do : create a commandlist specific for update resources on GPU and update when needed
	// push data to the GPU
	renderEngine.Render();

	GameObject * gameObject = game.CreateGameObject();
	gameObject->SetMesh(mesh);

	while (renderEngine.GetWindow().IsOpen())
	{
		float elapsedTime = game.TickFrame();
		// Update inputs and window position, and prepare command list to render
		renderEngine.UpdateWindow();

		// Update logic of the engine
		game.UpdateScene();

		// prepare buffer and command list for rendering
		renderEngine.PrepareForRender();

		// render objects on the scene
		game.RenderScene();

		// Execute command list
		renderEngine.Render();
	}

	// cleanup resources
	DX12RenderEngine::Delete();

	return 0;
}