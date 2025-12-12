#include "cameraclass.h"
#include "cmath"

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 3.0f, -4.0f);
	m_worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	m_yaw = 1.0f;
	m_pitch = 0.0f;

	m_mouseSensitivity = 0.02f;
	m_movementSpeed = 0.2f;

	m_firstMouse = true;
	m_mouseCaptured = false;

	UpdateVectors();
}

CameraClass::~CameraClass()
{
}

void CameraClass::Initialize(int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//center last mouse initially
	m_lastMouseX = screenWidth / 2;
	m_lastMouseY = screenHeight / 2;


	constexpr float fov = XMConvertToRadians(90.f);
	float aspectRatio = static_cast<float>(m_screenWidth) / static_cast<float>(m_screenHeight);
	float nearZ = 0.3f;
	float farZ = 1000.0f;

	m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

void CameraClass::UpdateVectors()
{
    // Calculate front vector from yaw and pitch angles
    XMFLOAT3 front;
    front.x = -cosf(m_yaw) * cosf(m_pitch);  // Negative X
    front.y = sinf(m_pitch);
    front.z = sinf(m_yaw) * cosf(m_pitch);

    // Normalize the front vector
    float length = sqrtf(front.x * front.x + front.y * front.y + front.z * front.z);
    m_front.x = front.x / length;
    m_front.y = front.y / length;
    m_front.z = front.z / length;

    // Calculate right vector: cross product of front and world up
    m_right.x = m_front.y * m_worldUp.z - m_front.z * m_worldUp.y;
    m_right.y = m_front.z * m_worldUp.x - m_front.x * m_worldUp.z;
    m_right.z = m_front.x * m_worldUp.y - m_front.y * m_worldUp.x;

    // Normalize right vector
    length = sqrtf(m_right.x * m_right.x + m_right.y * m_right.y + m_right.z * m_right.z);
    m_right.x /= length;
    m_right.y /= length;
    m_right.z /= length;

    // Calculate up vector: cross product of right and front
    m_up.x = m_right.y * m_front.z - m_right.z * m_front.y;
    m_up.y = m_right.z * m_front.x - m_right.x * m_front.z;
    m_up.z = m_right.x * m_front.y - m_right.y * m_front.x;
}

void CameraClass::UpdateViewMatrix()
{
    // Calculate target position (where camera is looking)
    DirectX::XMFLOAT3 target;
    target.x = m_position.x + m_front.x;
    target.y = m_position.y + m_front.y;
    target.z = m_position.z + m_front.z;

    // Create view matrix
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_position);
    DirectX::XMVECTOR targ = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);

    m_viewMatrix = DirectX::XMMatrixLookAtLH(pos, targ, up);
}

void CameraClass::HandleMouseMovement(int mouseX, int mouseY)
{
    if (m_firstMouse)
    {
        m_lastMouseX = mouseX;
        m_lastMouseY = mouseY;
        m_firstMouse = false;
        return;
    }

    // Calculate mouse movement difference
    float xOffset = (float)(m_lastMouseX - mouseX);
    float yOffset = (float)(mouseY - m_lastMouseY);

    // Store last mouse position
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    // Apply sensitivity
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    // Update yaw and pitch
    m_yaw -= xOffset;
    m_pitch -= yOffset;  // Note: minus because screen Y goes down, but we want up

    // Prevent camera from flipping (limit pitch)
    const float maxPitch = DirectX::XMConvertToRadians(89.0f);
    if (m_pitch > maxPitch)
        m_pitch = maxPitch;
    if (m_pitch < -maxPitch)
        m_pitch = -maxPitch;

    // Update camera vectors
    UpdateVectors();
    UpdateViewMatrix();
}


void CameraClass::MoveForward()
{
    // Move in the direction the camera is facing (front vector)
    m_position.x += m_front.x * m_movementSpeed;
    m_position.z += m_front.z * m_movementSpeed;  // Don't change Y to keep on ground
    UpdateViewMatrix();
}

void CameraClass::MoveBackward()
{
    m_position.x -= m_front.x * m_movementSpeed;
    m_position.z -= m_front.z * m_movementSpeed;
    UpdateViewMatrix();
}

void CameraClass::MoveLeft()
{
    // Move left relative to camera (use right vector, but negative)
    m_position.x += m_right.x * m_movementSpeed;
    m_position.z += m_right.z * m_movementSpeed;
    UpdateViewMatrix();
}

void CameraClass::MoveRight()
{
    m_position.x -= m_right.x * m_movementSpeed;
    m_position.z -= m_right.z * m_movementSpeed;
    UpdateViewMatrix();
}

void CameraClass::MoveUp(float deltaTime)
{
    // Move left relative to camera (use right vector, but negative)
    m_position.y += 20.0f * deltaTime;
    UpdateViewMatrix();
}

void CameraClass::MoveDown(float deltaTime)
{
    m_position.y -= 20.0f * deltaTime;
    UpdateViewMatrix();
}

void CameraClass::Update(float deltaTime)
{
    // Scale movement by deltaTime for framerate independence
    float speed = m_movementSpeed * deltaTime;

    // We'll handle key states in the main application
    // This function is for any continuous updates
}