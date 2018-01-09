#pragma once

#include <vector>
#include "DX12Camera.h"


class GameObject;

class GameScene
{
public:
	GameScene();
	~GameScene();

	// game scene management
	GameObject *		CreateGameObject(GameObject * i_Parent = nullptr);
	bool				DeleteGameObject(GameObject * i_GameObject, bool i_DeleteChild = true);

	// Render scene
	void				UpdateScene(float i_ElapsedTime) const;
	void				RenderScene() const;
	DX12Camera &		GetMainCamera();

	// friend class
	friend class GameObject;
private:
	// game object internal management
	void				UpdateGameObjectParenting(GameObject * i_GameObject);

	DX12Camera			m_Camera;	// camera used for rendering

	// Containers
	std::vector<GameObject *>		m_RootGameObjects;
	std::vector<GameObject *>		m_AllGameObjects;
};