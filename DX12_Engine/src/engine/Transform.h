// this class is used to simplify the game object position/rotation management
#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();
	Transform(XMFLOAT3 i_Translation, XMFLOAT3 i_Rotation = XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3 i_Scale = XMFLOAT3(1.f, 1.f, 1.f));
	~Transform();
	
	// position management
	void		SetPosition(const XMFLOAT3 & i_Position);
	void		Translate(const XMFLOAT3 & i_Translation);
	XMFLOAT3	GetPosition() const;

	// scale management
	void		SetScale(const XMFLOAT3 & i_Scale);
	XMFLOAT3	GetScale() const;

	// Retreive the matrix of transform
	XMFLOAT4X4		GetMatrix();
	XMFLOAT4X4		GetTransposed();

	// operator
	Transform &	operator=(const Transform i_Other);

private:
	// recompute
	void		RecomputeMatrix();

	// transform
	XMVECTOR	m_Position;
	XMVECTOR	m_Rotation;
	XMVECTOR	m_Scale;

	// manager
	bool		m_NeedToRecompute;

	// cached matrix
	XMFLOAT4X4		m_CacheMatrix;
	XMFLOAT4X4		m_CacheTransposed;
};