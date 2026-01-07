#include "CameraClass.h"

static constexpr float DEG2RAD = 0.0174532925f;

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 3.0f, 0.0f);

	m_pitch = 0.0f;
	m_yaw = 0.0f;

	m_rotationQuat = XMQuaternionIdentity();
	m_viewMatrix = XMMatrixIdentity();

	m_moveSpeed = 10.0f;
	m_mouseSensitivity = 0.2f;

	// Physics
	m_verticalVelocity = 0.0f;
	m_isGrounded = false;

	m_gravity = -9.81f * 4;
	m_jumpStrength = 10.0f;

	// Bounding box (player size)
	m_bounds = XMFLOAT3(0.4f, 0.9f, 0.4f);
	m_groundHeight = 3.0f;
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

XMFLOAT3 CameraClass::GetPosition() const
{
	return m_position;
}

XMVECTOR CameraClass::GetRotationQuat() const
{
	return m_rotationQuat;
}

void CameraClass::ProcessMouseInput(long mouseX, long mouseY)
{
	m_yaw += mouseX * m_mouseSensitivity;
	m_pitch += mouseY * m_mouseSensitivity;

	if (m_pitch > 89.0f)  m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	m_rotationQuat = XMQuaternionRotationRollPitchYaw(
		m_pitch * DEG2RAD,
		m_yaw * DEG2RAD,
		0.0f
	);
}


static XMVECTOR GetForwardVector(XMVECTOR quat)
{
	return XMVector3Rotate(
		XMVectorSet(0, 0, 1, 0),
		quat
	);
}

static XMVECTOR GetRightVector(XMVECTOR quat)
{
	return XMVector3Rotate(
		XMVectorSet(1, 0, 0, 0),
		quat
	);
}

void CameraClass::MoveForward(float deltaTime)
{
	XMVECTOR dir = GetForwardVector(m_rotationQuat);
	dir = XMVectorSetY(dir, 0.0f); // no flying
	dir = XMVector3Normalize(dir);

	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos += dir * (m_moveSpeed * deltaTime);
	XMStoreFloat3(&m_position, pos);
}

void CameraClass::MoveBackward(float deltaTime)
{
	XMVECTOR dir = GetForwardVector(m_rotationQuat);
	dir = XMVectorSetY(dir, 0.0f);
	dir = XMVector3Normalize(dir);

	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos -= dir * (m_moveSpeed * deltaTime);
	XMStoreFloat3(&m_position, pos);
}

void CameraClass::StrafeRight(float deltaTime)
{
	XMVECTOR dir = GetRightVector(m_rotationQuat);
	dir = XMVectorSetY(dir, 0.0f);
	dir = XMVector3Normalize(dir);

	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos += dir * (m_moveSpeed * deltaTime);
	XMStoreFloat3(&m_position, pos);
}

void CameraClass::StrafeLeft(float deltaTime)
{
	XMVECTOR dir = GetRightVector(m_rotationQuat);
	dir = XMVectorSetY(dir, 0.0f);
	dir = XMVector3Normalize(dir);

	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos -= dir * (m_moveSpeed * deltaTime);
	XMStoreFloat3(&m_position, pos);
}


void CameraClass::Jump()
{
	if (m_isGrounded)
	{
		m_verticalVelocity = m_jumpStrength;
		m_isGrounded = false;
	}
}

void CameraClass::UpdatePhysics(float deltaTime)
{
	m_verticalVelocity += m_gravity * deltaTime;

	m_position.y += m_verticalVelocity * deltaTime;

	// Simple ground collision
	float minY = m_groundHeight;

	if (m_position.y <= minY)
	{
		m_position.y = minY;
		m_verticalVelocity = 0.0f;
		m_isGrounded = true;
	}
}


void CameraClass::Render()
{
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR lookAt = XMVectorSet(0, 0, 1, 0);
	XMVECTOR position = XMLoadFloat3(&m_position);

	XMMATRIX rot = XMMatrixRotationQuaternion(m_rotationQuat);

	lookAt = XMVector3TransformCoord(lookAt, rot);
	up = XMVector3TransformCoord(up, rot);

	lookAt += position;

	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix) const
{
	viewMatrix = m_viewMatrix;
}
