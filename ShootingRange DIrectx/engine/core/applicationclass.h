#pragma once

//graphical includes
#include "../graphicsAPI/directx/d3dclass.h"
#include "../graphicsAPI/camera/cameraclass.h"
#include "../graphicsAPI/models/modelclass.h"
#include "../graphicsAPI/light/lightclass.h"
#include "../graphicsAPI/shadermanagerclass.h"
#include "../graphicsAPI/font/fontclass.h"
#include "../graphicsAPI/font/textclass.h"
#include "../graphicsAPI/models/modellistclass.h"
#include "../graphicsAPI/camera/positionclass.h"
#include "../graphicsAPI/frustum/frustumclass.h"

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
	bool UpdateRenderCountString(int);

	void HandleKeyboardInput(InputClass* Input);
	void ReadFileLocationsFromFile(std::vector<std::string>&, const std::string&);
private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	TimerClass* m_Timer;
	LightClass* m_Light;
	FontClass* m_Font;
	TextClass* m_RenderCountString;
	ModelListClass* m_ModelList;
	PositionClass* m_Position;
	FrustumClass* m_Frustum;
	XMMATRIX m_baseViewMatrix;
	ShaderManagerClass* m_ShaderManager;
};