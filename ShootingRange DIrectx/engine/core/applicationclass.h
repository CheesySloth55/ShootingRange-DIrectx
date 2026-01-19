#pragma once

//inlcudes
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>

//graphical includes
#include "../graphicsAPI/directx/d3dclass.h"
#include "../graphicsAPI/camera/cameraclass.h"
#include "../graphicsAPI/light/lightclass.h"
#include "../graphicsAPI/shadermanagerclass.h"
#include "../graphicsAPI/models/modellistclass.h"
#include "../graphicsAPI/camera/positionclass.h"
#include "../graphicsAPI/font/fontclass.h"
#include "../graphicsAPI/font/textclass.h"
#include "../graphicsAPI/frustum/frustumclass.h"
#include "../graphicsAPI/textures/rendertextureclass.h"
#include "../graphicsAPI/models/displayplaneclass.h"
#include "../graphicsAPI/models/meshclass.h"


//timer and input
#include "../peripheral/inputclass.h"
#include "../peripheral/timerclass.h"


//globals
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;
//const int maxObjectCount{ 1 }; // only used for modelList class
struct Bullet
{
	XMVECTOR position;
	XMVECTOR direction;
	float life;
	float speed;
	bool done;
};

class ApplicationClass
{
public:
	ApplicationClass() = default;
	ApplicationClass(const ApplicationClass&) = delete;
	~ApplicationClass() = default;

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame(InputClass* input);

	bool Render(float);
private:

	void HandleMouseMovement(InputClass& input, float);
	void HandleKeyboardInput(InputClass& Input, float);
	void ReadFileLocationsFromFile(std::vector<std::string>&, const std::string&);
private:
	std::unique_ptr<D3DClass> m_Direct3D{};
	std::unique_ptr<CameraClass> m_Camera{};
	std::unique_ptr<TimerClass> m_Timer{};
	std::unique_ptr<LightClass> m_Light{};
	//std::unique_ptr<FrustumClass> m_Frustum{};
	std::unique_ptr<FontClass>m_Font{};
	std::unique_ptr<TextClass>m_TextString{};
	std::unique_ptr<ShaderManagerClass> m_ShaderManager{};
	std::vector<std::unique_ptr<MeshClass>> m_MeshClasses{};
	HWND m_hwnd;
	int m_screenWidth;
	int m_screenHeight;
	int m_bulletCount{};
	float deltaTime{};
	std::vector<Bullet> m_bullets;
	XMMATRIX m_baseViewMatrix{};
	std::jthread m_renderThread;
	std::jthread m_frameThread;
};