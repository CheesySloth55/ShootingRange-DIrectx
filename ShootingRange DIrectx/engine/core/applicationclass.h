#pragma once

//graphical includes
#include "../graphicsAPI/directx/d3dclass.h"
#include "../graphicsAPI/camera/cameraclass.h"
#include "../graphicsAPI/models/modelclass.h"
#include "../graphicsAPI/shaders/specmapshaderclass.h"
#include "../graphicsAPI/light/lightclass.h"

//timer and input
#include "../peripheral/inputclass.h"
#include "../peripheral/timerclass.h"


//globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render(float);

	void HandleKeyboardInput(InputClass* Input);
private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	SpecMapShaderClass* m_SpecMapShader;
	ModelClass* m_Model;
	TimerClass* m_Timer;
	LightClass* m_Light;
};