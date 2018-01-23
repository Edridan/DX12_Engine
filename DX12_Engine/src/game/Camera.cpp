#include "Camera.h"

#include "dx12/DX12Utils.h"
#include "engine/Window.h"
#include "engine/Engine.h"

#include <math.h>

Camera::Camera(const XMMATRIX & i_Projection)
	:m_Position(0.f, 0.f, 0.f, 0.f)
	,m_Target(0.f, 0.f, 1.f, 0.f)
	,m_Freecam(false)
	,m_Pitch(0)
	,m_Yaw(0)
{
	XMStoreFloat4x4(&m_Projection, i_Projection);
}

Camera::Camera()
	:m_Position(0.f, 0.f, 0.f, 0.f)
	,m_Target(0.f, 0.f, 1.f, 0.f)
	,m_Freecam(false)
	,m_Pitch(0)
	,m_Yaw(0)
{
	// build projection matrix
	Window * wnd = Engine::GetInstance().GetWindow();

	const float windowRatio = (float)((float)wnd->GetWidth() / (float)wnd->GetHeight());
	XMMATRIX tmpProj = XMMatrixPerspectiveFovLH(45.f * (Pi / 180.f), windowRatio, 0.1f, 1000.f);

	XMStoreFloat4x4(&m_Projection, tmpProj);
}

Camera::~Camera()
{
}

void Camera::Update(FLOAT i_ElapsedTime)
{
	// build view matrix
	XMVECTOR cPos = XMLoadFloat4(&m_Position);
	XMVECTOR cTarg = XMLoadFloat4(&m_Target);
	XMVECTOR cUp = XMLoadFloat4(&m_Up);

	if (m_Freecam)
	{
		// manage free camera
		DirectX::XMVECTOR cForward = cTarg - cPos;
		DirectX::XMVECTOR cRight = DirectX::XMVector3Cross(cUp, cForward);
		cRight = DirectX::XMVector3Normalize(cRight);
		DirectX::XMFLOAT3 velocity = { 0.f, 0.f, 0.f };
		const static FLOAT speed = 10.f;

		// retreive the input concerning mouse
		Window * window = Engine::GetInstance().GetWindow();

		// forward/backward
		if		(GetAsyncKeyState('Z'))		velocity.z = speed;
		else if (GetAsyncKeyState('S'))		velocity.z = -speed;
		// right/left
		if		(GetAsyncKeyState('D'))		velocity.x = speed;
		else if (GetAsyncKeyState('Q'))		velocity.x = -speed;
		// up/down
		if		(GetAsyncKeyState('E'))		velocity.y = speed;
		else if (GetAsyncKeyState('A'))		velocity.y = -speed;

		// the right button of the mouse is pressed
		if (GetAsyncKeyState(VK_RBUTTON))
		{
			// To do : optimize this part of code, need good math here
			// need to fix strange deformation of the screen's edges
			static const float Sensivity = 0.1f;
			// Retreive the current pitch and yaw of the camera
			IntVec2 mouseMovement = window->GetMouseMove();
			DirectX::XMFLOAT3 vForward;
			XMStoreFloat3(&vForward, cForward);

			// retreive the current pitch/yaw based on forward vector
			m_Yaw = atan2f(vForward.x, vForward.z) * RadToDeg;
			m_Pitch = asinf(-vForward.y) * RadToDeg;

			m_Pitch += mouseMovement.y * Sensivity;
			m_Yaw	+= mouseMovement.x * Sensivity;

			if (m_Pitch > 85.0f)	m_Pitch = 85.0f;
			if (m_Pitch < -85.0f)	m_Pitch = -85.0f;

			const float cosPitch = cosf(m_Pitch * DegToRad);
			const float sinPitch = sinf(m_Pitch * DegToRad);
			const float cosYaw = cosf(m_Yaw * DegToRad);
			const float sinYaw = sinf(m_Yaw * DegToRad);

			vForward.x = cosPitch * sinYaw;
			vForward.y = -sinPitch;
			vForward.z = cosPitch * cosYaw;

			cForward = XMLoadFloat3(&vForward);
			cTarg = cPos + cForward;
		}

		// create velocity
		DirectX::XMVECTOR cVelocity = XMLoadFloat3(&velocity);
		cVelocity = cForward * velocity.z;
		cVelocity += cRight * velocity.x;
		cVelocity += cUp * velocity.y;

		cVelocity *= i_ElapsedTime;

		// update positon of the camera
		cPos += cVelocity;
		cTarg += cVelocity;

		// update vectors after transforms to be computed
		XMStoreFloat4(&m_Position, cPos);
		XMStoreFloat4(&m_Target, cTarg);
	}

	XMMATRIX tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
	XMStoreFloat4x4(&m_View, tmpMat);
}

void Camera::SetProjectionMatrix(const XMMATRIX & i_Projection)
{
	XMStoreFloat4x4(&m_Projection, i_Projection);
}

XMFLOAT4 Camera::GetFoward() const
{
	const XMVECTOR cPos = XMLoadFloat4(&m_Position);
	const XMVECTOR cTarg = XMLoadFloat4(&m_Target);
	const XMVECTOR cForward = (cTarg - cPos);

	XMFLOAT4 forward;
	XMStoreFloat4(&forward, cForward);

	return forward;
}

XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return m_View;
}

XMFLOAT4X4 Camera::GetProjMatrix() const
{
	return m_Projection;
}

void Camera::SetFreecamEnabled(bool i_Enabled)
{
	m_Freecam = i_Enabled;
}

bool Camera::FreecamIsEnabled() const
{
	return m_Freecam;
}
