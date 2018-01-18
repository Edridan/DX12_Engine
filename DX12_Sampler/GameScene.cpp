#include "GameScene.h"

// game
#include "GameObject.h"

// DX12 engine
#include "DX12RenderEngine.h"
#include "DX12Utils.h"

GameScene::GameScene(const GameSceneDesc & i_GameSceneDesc)
	:m_AllGameObjects()
	,m_RootGameObjects()
{
	// setup default
	m_Camera.m_Position = i_GameSceneDesc.CameraPosition;
	m_Camera.m_Target =i_GameSceneDesc.CameraTarget;

	m_Camera.m_Up = XMFLOAT4(0.f, 1.f, 0.f, 0.f);

	if (i_GameSceneDesc.UseCameraProjection)
	{
		m_Camera.SetProjectionMatrix(i_GameSceneDesc.CameraProjection);
	}

	// debug mode activated
	m_Camera.SetFreecamEnabled(true);
}

GameScene::~GameScene()
{
	// delete game objects in the scene
	for (unsigned int i = 0; i < m_AllGameObjects.size(); ++i)
	{
		delete m_AllGameObjects[i];
	}

	m_AllGameObjects.clear();
	m_RootGameObjects.clear();
}

GameObject * GameScene::CreateGameObject(GameObject * i_Parent)
{
	GameObject * newGameObject = new GameObject(this);

	m_AllGameObjects.push_back(newGameObject);

	// Add the game object to the root
	if (newGameObject->IsRoot())
	{
		m_RootGameObjects.push_back(newGameObject);
	}

	return newGameObject;
}

bool GameScene::DeleteGameObject(GameObject * i_GameObject, bool i_DeleteChild)
{
	// remove the object from root
	if (i_GameObject->IsRoot())
	{
		auto itr = m_RootGameObjects.begin();
		bool found = false;

		while ((itr != m_RootGameObjects.end()) || (!found))
		{
			if ((*itr) == i_GameObject)
			{
				m_RootGameObjects.erase(itr);
				found = true;
			}

			++itr;
		}
	}

	if (!i_DeleteChild)
	{
		// update child
		i_GameObject->RemoveChildren();
	}
	else
	{
		for (unsigned int i = 0; i < i_GameObject->m_Child.size(); ++i)
		{
			DeleteGameObject(i_GameObject->m_Child[i], true);
		}
	}

	// Remove the game object from the list
	auto itr = m_AllGameObjects.begin();

	while (itr != m_AllGameObjects.end())
	{
		if ((*itr) == i_GameObject)
		{
			m_AllGameObjects.erase(itr);
			return true;
		}
		++itr;
	}
	
	return false;
}

void GameScene::UpdateScene(float i_ElapsedTime)
{
	// Update camera
	m_Camera.Update(i_ElapsedTime);

	auto itr = m_RootGameObjects.begin();

	while (itr != m_RootGameObjects.end())
	{
		GameObject * gameObject = (*itr);
		gameObject->Update(i_ElapsedTime);

		++itr;
	}
}

void GameScene::RenderScene() const
{
	// Render stuff here
	ID3D12GraphicsCommandList * commandList = DX12RenderEngine::GetInstance().GetCommandList();
	auto itr = m_RootGameObjects.begin();

	while (itr != m_RootGameObjects.end())
	{
		GameObject * gameObject = (*itr);
		gameObject->Render(commandList);

		++itr;
	}
}

DX12Camera & GameScene::GetMainCamera()
{
	return m_Camera;
}

void GameScene::UpdateGameObjectParenting(GameObject * i_GameObject)
{
	const bool isRoot = i_GameObject->IsRoot();
	bool found = false;
	auto itr = m_RootGameObjects.begin();

	while ((itr != m_RootGameObjects.end()) || (!found))
	{
		// We found the game object
		if ((*itr) == i_GameObject)
		{
			found = true;
		}

		++itr;
	}

	if (!found && isRoot)
	{
		m_RootGameObjects.push_back(i_GameObject);
	}
	else if (found && !isRoot)
	{
		m_RootGameObjects.erase(itr);
	}
}
