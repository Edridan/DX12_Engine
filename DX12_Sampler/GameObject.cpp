#include "GameObject.h"

#include "GameScene.h"

// DX12
#include "DX12Mesh.h"
#include "DX12MeshBuffer.h"

// initialize game object
GameObject::GameObject(GameScene * const i_Scene)
	:m_Mesh(nullptr)
	,m_Child()
	,m_Parent(nullptr)
	,m_Transform(DirectX::XMMatrixIdentity())
	,m_Scene(i_Scene)
	,m_ConstBuffer((ADDRESS_ID)-1)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(float i_ElapsedTime)
{
}

void GameObject::Render(ID3D12GraphicsCommandList * i_CommandList)
{
	if (CanBeRendered())
	{
		DX12RenderEngine & render = DX12RenderEngine::GetInstance();

		// update the model view transform matrix
		DX12RenderEngine::DefaultConstantBuffer constantBuffer;
		DX12Camera & cam = m_Scene->GetMainCamera();

		XMMATRIX viewMat = XMLoadFloat4x4(&cam.GetViewMatrix()); // load view matrix
		XMMATRIX projMat = XMLoadFloat4x4(&cam.GetProjMatrix()); // load projection matrix
		XMMATRIX mvpMatrix = GetWorldTransform() * viewMat * projMat; // create mvp matrix
		XMMATRIX transposed = XMMatrixTranspose(mvpMatrix);

		// update data into the const buffer
		XMStoreFloat4x4(&constantBuffer.m_Transform, transposed);
		DX12RenderEngine::GetInstance().UpdateConstantBuffer(m_ConstBuffer, constantBuffer);

		// render mesh
		// add pso and root signature to the commandlist
		i_CommandList->SetGraphicsRootSignature(render.GetRootSignature());
		
		// push const buffer
		if (m_ConstBuffer != UnavailableAdressId)
		{
			i_CommandList->SetGraphicsRootConstantBufferView(0, render.GetConstantBufferUploadVirtualAddress(m_ConstBuffer));
		}

		// Setup the pipeline state
		i_CommandList->SetPipelineState(m_PipelineState);

		// push the mesh on the commandlist
		m_Mesh->PushOnCommandList(i_CommandList);
	}

	// render childs gameobjects
	auto itr = m_Child.begin();
	while (itr != m_Child.end())
	{
		(*itr)->Render(i_CommandList);
		++itr;
	}
}

void GameObject::SetMesh(DX12Mesh * i_Mesh, ID3D12PipelineState * i_PipelineState)
{
	if (i_Mesh == nullptr)	return;

	// setup the root mesh into the game object
	SetMeshBuffer(i_Mesh->GetRootMesh(), i_PipelineState);

	/*for (UINT32 subMesh = 0; i_Mesh->SubMeshesCount(); ++subMesh)
	{
		GameObject * subGameObject = m_Scene->CreateGameObject(this);
	}*/
}

void GameObject::SetMeshBuffer(const DX12MeshBuffer * i_MeshBuffer, ID3D12PipelineState * i_RenderState)
{
	// setup mesh 
	m_Mesh = i_MeshBuffer;
	m_PipelineState = i_RenderState;

	if (m_Mesh && (m_ConstBuffer == UnavailableAdressId))
	{
		// map gpu const buffer address if necessary
		// this allow the game object to put data into the const buffer
		m_ConstBuffer = DX12RenderEngine::GetInstance().ReserveConstantBufferVirtualAddress();
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

bool GameObject::CanBeRendered() const
{
	return ((m_Mesh != NULL) && (m_PipelineState != NULL));
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
