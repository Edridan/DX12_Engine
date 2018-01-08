// Camera management

#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class DX12Camera
{
public:
	DX12Camera(XMMATRIX i_Projection);
	DX12Camera();
	~DX12Camera();

	// Camera management
	void			Update();

	// camera management
	// these are XMFLOAT4 to render directly use the function or translate these vector to XMVECTOR in order to do math
	XMFLOAT4	m_Position;
	XMFLOAT4	m_Target;
	XMFLOAT4	m_Up;

	XMFLOAT4X4	GetViewMatrix() const;
	XMFLOAT4X4	GetProjMatrix() const;

private:
	// Matrix
	XMFLOAT4X4	m_Projection;
	XMFLOAT4X4	m_View;

	
};