// A Game Object is a transform with other specific for rendering/updating

#pragma once

#include <vector>

#include "DX12Mesh.h"
#include "DX12ConstantBuffer.h"

class GameScene;

class GameObject
{
public:
	// game object management
	void		SetMesh(DX12Mesh * i_Mesh, ID3D12PipelineState * i_RenderState);

	// information
	bool	IsRoot() const;
	bool	IsEmpty() const;
	bool	HaveChild() const;
	bool	NeedRendering() const;

	// parenting
	void	SetParent(GameObject * i_Parent);
	bool	AddChild(GameObject * i_Child);
	bool	RemoveChild(GameObject * i_Remove);
	void	RemoveChildren();

	DirectX::XMMATRIX		GetWorldTransform() const;

	// transform of the object
	DirectX::XMMATRIX		m_Transform;

	// friend class
	friend class GameScene;
private:
	// created and deleted by the game scene
	GameObject(GameScene * const i_Scene);
	~GameObject();

	virtual void	Update(float i_ElapsedTime);
	void			Render(ID3D12GraphicsCommandList* i_CommandList);

	// game object attributes
	// rendering pipeline stuff
	DX12Mesh *				m_Mesh;
	ID3D12PipelineState *	m_PipelineState;
	ADDRESS_ID				m_ConstBuffer;

	GameScene * const	m_Scene;

	// parenting system
	GameObject *				m_Parent;
	std::vector<GameObject *>	m_Child;
};