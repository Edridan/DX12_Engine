#include "engine/Transform.h"

Transform::Transform()
	:m_NeedToRecompute(true)
{
	m_Position	= DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	m_Scale		= DirectX::XMLoadFloat3(&XMFLOAT3(1, 1, 1));
	m_Rotation	= DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 0));
}

Transform::~Transform()
{
}

void Transform::SetPosition(const XMFLOAT3 & i_Position)
{
	m_Position = DirectX::XMLoadFloat3(&i_Position);
}

void Transform::Translate(const XMFLOAT3 & i_Translation)
{
	XMVECTOR translation = DirectX::XMLoadFloat3(&i_Translation);
	m_Position += translation;
}

XMFLOAT3 Transform::GetPosition() const
{
	XMFLOAT3 vRet;
	DirectX::XMStoreFloat3(&vRet, m_Position);
	return vRet;
}

void Transform::SetScale(const XMFLOAT3 & i_Scale)
{
	m_Scale = DirectX::XMLoadFloat3(&i_Scale);
}

XMFLOAT3 Transform::GetScale() const
{
	XMFLOAT3 vRet;
	DirectX::XMStoreFloat3(&vRet, m_Scale);
	return vRet;
}

XMFLOAT4X4 Transform::GetMatrix()
{
	if (m_NeedToRecompute)
	{
		// recompute matrix here
		RecomputeMatrix();
		m_NeedToRecompute = false;
	}

	return m_CacheMatrix;
}

XMFLOAT4X4 Transform::GetTransposed()
{
	if (m_NeedToRecompute)
	{
		// recompute matrix here
		RecomputeMatrix();
		m_NeedToRecompute = false;
	}

	return m_CacheTransposed;
}

// recompute matrix here
inline void Transform::RecomputeMatrix()
{
	// compute rotation matrix
	XMFLOAT3 tRot;
	DirectX::XMStoreFloat3(&tRot, m_Rotation);

	XMMATRIX rotXMat = XMMatrixRotationX(tRot.x);
	XMMATRIX rotYMat = XMMatrixRotationY(tRot.y);
	XMMATRIX rotZMat = XMMatrixRotationZ(tRot.z);

	XMMATRIX rotMat = rotXMat * rotYMat * rotZMat;

	// compute scale matrix
	XMFLOAT3 tScale;
	DirectX::XMStoreFloat3(&tScale, m_Scale);

	XMMATRIX scaleMat = XMMatrixScaling(tScale.x, tScale.y, tScale.z);

	// compute translation matrix
	XMFLOAT3 tPos;
	DirectX::XMStoreFloat3(&tPos, m_Position);

	XMMATRIX posMat = XMMatrixTranslation(tPos.x, tPos.y, tPos.z);

	XMMATRIX mat = rotMat *posMat * scaleMat;

	// compute matrix
	XMStoreFloat4x4(&m_CacheMatrix, mat);
	XMStoreFloat4x4(&m_CacheTransposed, XMMatrixTranspose(mat));
}
