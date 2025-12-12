#pragma once

#include <directxmath.h>
using namespace DirectX;


class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void Initialize(int screenWidth, int screenHeight);

	void Update(float deltaTime);
	void HandleMouseMovement(int mouseX, int mouseY);

	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();
	void MoveUp(float deltaTime);
	void MoveDown(float deltaTime);

	XMFLOAT3 GetPosition() const { return m_position; };

	XMMATRIX GetViewMatrix() const { return m_viewMatrix; }
	XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }

	void UpdateViewMatrix();
private:
	void UpdateVectors();

private:
	//position and rotation
	XMFLOAT3 m_position;

	XMFLOAT4 m_rotationQuaternion;

	float m_yaw;    // Left/right rotation
	float m_pitch; // up/down rotation
	
	XMFLOAT3 m_front;
	XMFLOAT3 m_up;
	XMFLOAT3 m_right;
	XMFLOAT3 m_worldUp;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	int m_screenWidth;
	int m_screenHeight;
	int m_lastMouseX;
	int m_lastMouseY;
	bool m_firstMouse;

	float m_mouseSensitivity;
	float m_movementSpeed;
	bool m_mouseCaptured;
};

