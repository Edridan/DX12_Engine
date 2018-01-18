// Code for DX12 sampler tutorial


// Engine
#include "DX12RenderEngine.h"
#include "DX12Mesh.h"

// Game
#include "GameScene.h"
#include "GameObject.h"
#include "Clock.h"

// DX12
#include <DirectXMath.h>
#include "d3dx12.h"


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
	GameScene game;
	Clock time;	// time for frame

	DX12Mesh * mesh = DX12Mesh::LoadMeshObj("resources/obj/cube.obj", "resources/obj/");
	DX12Mesh * cube = DX12Mesh::GeneratePrimitiveMesh(DX12Mesh::eTriangle);

	// push data to the GPU
	renderEngine.Render();

	GameObject * gameObject = game.CreateGameObject();
	/*gameObject->SetMesh(cube);

	gameObject = game.CreateGameObject();*/
	gameObject->SetMesh(mesh);

	while (renderEngine.GetWindow().IsOpen())
	{
		float elapsedTime = time.Restart().ToSeconds();
		// Update inputs and window position, and prepare command list to render
		renderEngine.UpdateWindow();

		// Update logic of the engine
		game.UpdateScene(elapsedTime);

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