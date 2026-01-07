#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass& other) = delete;
	~CameraClass() = default;

	// Position
	void SetPosition(float x, float y, float z);
	XMFLOAT3 GetPosition() const;
	XMVECTOR GetRotationQuat() const;

	void MoveForward(float deltaTime);
	void MoveBackward(float deltaTime);
	void StrafeRight(float deltaTime);
	void StrafeLeft(float deltaTime);

	void Jump();
	void UpdatePhysics(float deltaTime);

	// Mouse input (DirectInput8)
	void ProcessMouseInput(long mouseX, long mouseY);

	// Build view matrix
	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix) const;

private:
	// Position
	XMFLOAT3 m_position;

	// Euler rotation (degrees)
	float m_pitch; // X
	float m_yaw;   // Y

	// Quaternion rotation (internal)
	XMVECTOR m_rotationQuat;
	XMMATRIX m_viewMatrix;

	// Movement
	float m_moveSpeed;
	float m_mouseSensitivity;

	// Physics
	float m_verticalVelocity;
	bool  m_isGrounded;

	float m_gravity;
	float m_jumpStrength;

	// Bounding box (half extents)
	XMFLOAT3 m_bounds;
	float    m_groundHeight;
};
