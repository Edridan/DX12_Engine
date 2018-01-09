#include "GameObject.h"

#include "GameScene.h"

// initialize game object
GameObject::GameObject(GameScene * const i_Scene)
	:m_Mesh(nullptr)
	,m_Child()
	,m_Parent(nullptr)
	,m_Transform(DirectX::XMMatrixIdentity())
	,m_Scene(i_Scene)
{
}

GameObject::~GameObject()
{
}

void GameObject::Render(ID3D12GraphicsCommandList * i_CommandList)
{
	if (NeedRendering())
	{
		// update the model view transform matrix
		DX12Camera & cam = m_Scene->GetMainCamera();

		XMMATRIX viewMat = XMLoadFloat4x4(&cam.GetViewMatrix()); // load view matrix
		XMMATRIX projMat = XMLoadFloat4x4(&cam.GetProjMatrix()); // load projection matrix
		XMMATRIX mvpMatrix = GetWorldTransform() * viewMat * projMat; // create mvp matrix

		XMStoreFloat4x4(&m_ConstantBuffer.m_ModelViewProjMat, mvpMatrix);

		// draw
		m_Mesh->Draw(i_CommandList, m_PipelineState);
	}

	auto itr = m_Child.begin();
	while (itr != m_Child.end())
	{
		(*itr)->Render(i_CommandList);
		++itr;
	}
}

void GameObject::SetMesh(DX12Mesh * i_Mesh, ID3D12PipelineState * i_PipelineState)
{
	m_Mesh = i_Mesh;
	m_PipelineState = i_PipelineState;

	if (i_Mesh)
	{
		// map gpu address if necessary
		//m_ConstantBufferArea.MapGPUAddress();
	}
}

bool GameObject::IsRoot() const
{
	return (m_Parent == nullptr);
}

bool GameObject::IsEmpty() const
{
	return (m_Mesh != nullptr);
}

bool GameObject::HaveChild() const
{
	return (m_Child.size() > 0);
}

bool GameObject::NeedRendering() const
{
	return (m_Mesh != nullptr);
}

void GameObject::SetParent(GameObject * i_Parent)
{
	// changing parenting
	if (m_Parent)
		m_Parent->RemoveChild(this);

	m_Parent = i_Parent;

	if (m_Parent)
		m_Parent->SetParent(this);

	m_Scene->UpdateGameObjectParenting(this);
}

bool GameObject::AddChild(GameObject * i_Child)
{
	m_Child.push_back(i_Child);
	i_Child->m_Parent = this;

	return true;
}

bool GameObject::RemoveChild(GameObject * i_Remove)
{
	auto itr = m_Child.begin();

	while (itr != m_Child.end())
	{
		// we found the child in the game object list
		if ((*itr) == i_Remove)
		{
			(*itr)->SetParent(nullptr);
			m_Child.erase(itr);
			return true;
		}

		++itr;
	}

	// we didn't find the child in the game object list
	return false;
}

void GameObject::RemoveChildren()
{
	auto itr = m_Child.begin();

	while (itr != m_Child.end())
	{
		// update child game object
		(*itr)->SetParent(nullptr);
		m_Scene->UpdateGameObjectParenting(*itr);

		++itr;
	}

}

DirectX::XMMATRIX GameObject::GetWorldTransform() const
{ 
	if (m_Parent != nullptr)
	{
		return m_Parent->GetWorldTransform() * m_Transform;
	}

	return m_Transform;
}
