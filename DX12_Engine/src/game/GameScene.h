#pragma once

#include <vector>
#include "Camera.h"
#include "engine/Clock.h"

class GameObject;

class GameScene
{
public:
	struct GameSceneDesc
	{
		// initial camera position
		XMFLOAT4		CameraPosition				= XMFLOAT4(0.f, 0.f, -1.f, 1.f);
		XMFLOAT4		CameraTarget				= XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		bool			UseCameraProjection			= false;
		XMMATRIX		CameraProjection			= XMMatrixIdentity();

	};

	GameScene(const GameSceneDesc & i_GameSceneDesc);
	~GameScene();

	// game scene management
	GameObject *		CreateGameObject(GameObject * i_Parent = nullptr);
	bool				DeleteGameObject(GameObject * i_GameObject, bool i_DeleteChild = true);

	// game scene information
	float				TickFrame();
	float				GetLiveTime() const;

	// Render scene
	void				UpdateScene();
	void				RenderScene() const;
	Camera &			GetMainCamera();

	// friend class
	friend class GameObject;
private:
	// game object internal management
	void				UpdateGameObjectParenting(GameObject * i_GameObject);

	Camera				m_Camera;		// camera used for rendering
	Clock				m_GameClock;	// game clock
	float				m_ElapsedTime;


	// Containers
	std::vector<GameObject *>		m_RootGameObjects;
	std::vector<GameObject *>		m_AllGameObjects;
};