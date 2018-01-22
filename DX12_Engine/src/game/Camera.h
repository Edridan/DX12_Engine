// Camera management

#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

using namespace DirectX;

class Camera
{
public:
	Camera(const XMMATRIX & i_Projection);
	Camera();
	~Camera();

	// camera management
	void			Update(FLOAT i_ElapsedTime);

	XMFLOAT4	m_Position;
	XMFLOAT4	m_Target;
	XMFLOAT4	m_Up;

	void		SetProjectionMatrix(const XMMATRIX & i_Projection);

	XMFLOAT4	GetFoward() const;

	XMFLOAT4X4	GetViewMatrix() const;
	XMFLOAT4X4	GetProjMatrix() const;

	// freecam
	void	SetFreecamEnabled(bool i_Enabled);
	bool	FreecamIsEnabled() const;

private:
	// Matrix
	XMFLOAT4X4	m_Projection;
	XMFLOAT4X4	m_View;

	// freecam management
	bool		m_Freecam;
	FLOAT		m_Yaw, m_Pitch;
	FLOAT		m_Sensivity;
};