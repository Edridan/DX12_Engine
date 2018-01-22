// main definition
// create the engine and run it
#include "engine/Engine.h"
// engine game class
#include "game/World.h"
#include "game/Actor.h"
// WinMain
#include <Windows.h>

int WINAPI WinMain(
	HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	Engine::EngineDesc desc;

	// define the engine setup
	desc.HInstance = hInstance;

	// create the engine singleton
	Engine::Create();
	Engine & engine = Engine::GetInstance();

	// initialize and run the engine
	engine.Initialize(desc);

	// initialize the world
	World * world = engine.GetWorld();

	// create objects...
	Actor::ActorDesc actorDesc;
	actorDesc.Mesh = L"resources/obj/cube.obj";

	world->SpawnActor(actorDesc);

	engine.Run();
}