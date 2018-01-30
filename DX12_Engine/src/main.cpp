// main definition
// create the engine and run it
#include "engine/Engine.h"
// engine game class
#include "game/World.h"
#include "game/Actor.h"
// WinMain
#include <Windows.h>

// debug : to delete
#include "engine/ResourcesManager.h"

int WINAPI WinMain(
	HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	Engine::EngineDesc desc;

	// define the engine setup
	desc.HInstance				= hInstance;
	desc.FramePerSecondTargeted = 60;
	desc.CameraPosition = XMFLOAT4(0.f, 0.f, -5.f, 0.f);

	// create the engine singleton
	Engine::Create();
	Engine & engine = Engine::GetInstance();

	// initialize and run the engine
	engine.Initialize(desc);

	// initialize the world
	World * world = engine.GetWorld();

	// create objects...
	Actor::ActorDesc cubeActor;
	cubeActor.Name = L"Box";
	//cubeActor.Mesh = L"resources/obj/cube.obj";
	cubeActor.Mesh = L"Plane";
	cubeActor.ActorTransform = Transform(XMFLOAT3(0.f, 0.f, 0.f));

	Actor * actor = world->SpawnActor(cubeActor);
	DX12Texture * tex = engine.GetResourcesManager()->GetTexture(L"resources/tex/brick_messy.jpg");
	actor->GetRenderComponent()->SetTexture(tex);

	cubeActor.ActorTransform = Transform(XMFLOAT3(2.f, 0.f, 0.f));

	actor = world->SpawnActor(cubeActor);

	tex = engine.GetResourcesManager()->GetTexture(L"resources/tex/brick_grouthless.jpg");
	actor->GetRenderComponent()->SetTexture(tex);

	engine.Run();
}