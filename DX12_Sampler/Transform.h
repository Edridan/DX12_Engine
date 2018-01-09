// this class is used to simplify the game object position/rotation management
#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();
	~Transform();


	XMVECTOR	m_Position;
	XMVECTOR	m_Rotation;
	XMVECTOR	m_Scale;

	XMMATRIX		GetMatrix() const;
};