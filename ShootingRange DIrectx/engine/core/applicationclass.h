#pragma once

//inlcudes
#include <memory>
#include <vector>
#include <string>
#include <fstream>

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
#include "../graphicsAPI/textures/rendertextureclass.h"
#include "../graphicsAPI/models/displayplaneclass.h"



//timer and input
#include "../peripheral/inputclass.h"
#include "../peripheral/timerclass.h"


//globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;
const int maxObjectCount{ 1 };


class ApplicationClass
{
public:
	ApplicationClass() = default;
	ApplicationClass(const ApplicationClass&) = delete;
	~ApplicationClass() = default;

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame(InputClass* input);

private:
	bool Render();
	bool RenderSceneToTexture(float);
	bool UpdateRenderCountString(int renderCount);

	void HandleKeyboardInput(InputClass* Input);
	void ReadFileLocationsFromFile(std::vector<std::string>&, const std::string&);
private:
	std::unique_ptr<D3DClass> m_Direct3D{};
	std::unique_ptr<CameraClass> m_Camera{};
	std::unique_ptr<ModelClass> m_Model{};
	std::unique_ptr<ModelClass> m_Model2{};
	std::unique_ptr<TimerClass> m_Timer{};
	std::unique_ptr<LightClass> m_Light{};
	std::unique_ptr<FontClass> m_Font{};
	std::unique_ptr<TextClass> m_RenderCountString{};
	std::unique_ptr<ModelListClass> m_ModelList{};
	std::unique_ptr<PositionClass>m_Position{};
	std::unique_ptr<FrustumClass> m_Frustum{};
	std::unique_ptr<ShaderManagerClass> m_ShaderManager{};
	std::unique_ptr<RenderTextureClass> m_RenderTexture{};
	std::unique_ptr<DisplayPlaneClass> m_DisplayPlane{};

	XMMATRIX m_baseViewMatrix{};
};