#include "DX12Camera.h"

#include "DX12RenderEngine.h"
#include "DX12Window.h"

DX12Camera::DX12Camera(XMMATRIX i_Projection)
	:m_Position(0.f, 0.f, 0.f, 0.f)
	,m_Target(0.f, 0.f, 1.f, 0.f)
{
	XMStoreFloat4x4(&m_Projection, i_Projection);
}

DX12Camera::DX12Camera()
	:m_Position(0.f, 0.f, 0.f, 0.f)
	,m_Target(0.f, 0.f, 1.f, 0.f)
{
	// build projection matrix
	DX12Window & wnd = DX12RenderEngine::GetInstance().GetWindow();

	const int windowRatio = wnd.GetWidth() / wnd.GetHeight();
	XMMATRIX tmpProj = XMMatrixPerspectiveFovLH(90.f, windowRatio, 0.05f, 1000.f);

	XMStoreFloat4x4(&m_Projection, tmpProj);
}

DX12Camera::~DX12Camera()
{
}

void DX12Camera::Update()
{
	// build view matrix
	XMVECTOR cPos = XMLoadFloat4(&m_Position);
	XMVECTOR cTarg = XMLoadFloat4(&m_Target);
	XMVECTOR cUp = XMLoadFloat4(&m_Up);

	XMMATRIX tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);

	XMStoreFloat4x4(&m_View, tmpMat);
}

XMFLOAT4X4 DX12Camera::GetViewMatrix() const
{
	return m_View;
}

XMFLOAT4X4 DX12Camera::GetProjMatrix() const
{
	return m_Projection;
}
