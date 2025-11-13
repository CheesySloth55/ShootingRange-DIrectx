
#include "applicationclass.h"
#include <vector>
#include <string>
#include <fstream>

ApplicationClass::ApplicationClass()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
    m_ShaderManager = NULL;
    m_Light = NULL;
    m_Timer = NULL;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    std::vector<std::string> modelFilenames;
    std::vector<std::string> textureFilenames;
    const std::string texturelistLocation{"engine/data/texturelist.txt"};
    bool result;

    ReadFileLocationsFromFile(textureFilenames, texturelistLocation);

    // Create and initialize the Direct3D object.
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the camera object.
    m_Camera = new CameraClass;

    // camera points by default at the z direction
    m_Camera->SetPosition(0.0f, 10.0f, -8.0f);
    m_Camera->Render();

    // Create and initialize the specular map shader object.


    //model data
    modelFilenames.push_back("engine/data/model-data/square.txt");
    //textures



    // Create and initialize the model object.
    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[0], textureFilenames);
    if (!result)
    {
        return false;
    }

    //initialize timer class;
    m_Timer = new TimerClass;

    result = m_Timer->Initialize();
    if(!result)
    {
        return false;
    }

    // Create and initialize the light object.
    m_Light = new LightClass;

    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(10.0f);

    m_ShaderManager = new ShaderManagerClass;

    result = m_ShaderManager->Intialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }
    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the model object.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    if (m_ShaderManager)
    {
        m_ShaderManager->Shutdown();
        delete m_ShaderManager;
        m_ShaderManager = 0;
    }

    // Release the light object.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    //release timer object
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Release the camera object.
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // Release the Direct3D object.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
    bool result;
    double frameTime;
    static float rotation = 360.0f;
    static const int physicsFrametime = 10; // every 10ms update physics
    static double physicsTimeAccumulator = 0.0; // accumulated time since last physics update

    m_Timer->Frame();

    // Get the current frame time.
    frameTime = m_Timer->GetFrameTime();
    physicsTimeAccumulator += frameTime * 1000.0f; // convert to ms

    // Update physics only if enough time has passed
    while (physicsTimeAccumulator >= physicsFrametime)
    {
        rotation -= 0.0174532925f * 0.5f ; // 1 degree in radians
        if (rotation <= 0.0f)
        {
            rotation += 360.0f;
        }
        physicsTimeAccumulator -= physicsFrametime;
    }

    // Check if the user pressed escape and wants to exit the application.
    if (Input->IsEscapePressed())
    {
        return false;
    }

    HandleKeyboardInput(Input);

    result = Render(rotation);
    if (!result)
    {
        return false;
    }

    return true;
}



bool ApplicationClass::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();


    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);


    for (int i{ 0 }; i < 10; i++)
    {
        for (int j{ 0 }; j < 10; j++)
        {
            rotateMatrix = XMMatrixRotationY(rotation);
            translateMatrix = XMMatrixTranslation(-1.5f - i * 2.5f, -1.0f + j * 2.5f, 0.0f);
            worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

            // Render the model using the normal map shader.
            m_Model->Render(m_Direct3D->GetDeviceContext());

            result = m_ShaderManager->RenderMultiTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
                m_Model->GetTexture(3), m_Model->GetTexture(4), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
            if (!result)
            {
                return false;
            }
        }
    }


    m_Direct3D->EndScene();

    return true;
}


void ApplicationClass::HandleKeyboardInput(InputClass* Input)
{
    static float movementSpeed = 0.05;
    if (Input->IsF11Pressed())
    {
        m_Direct3D->ToggleFullScreenMode();
    }

    if (Input->IsUPArrowPressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Camera->GetPosition();
        m_Camera->SetPosition(curPos.x, curPos.y, curPos.z + movementSpeed);
        m_Camera->Render();
    }

    if (Input->IsDOWNArrowPressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Camera->GetPosition();
        m_Camera->SetPosition(curPos.x, curPos.y, curPos.z - movementSpeed);
        m_Camera->Render();
    }

    if (Input->IsLEFTArrowPressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Camera->GetPosition();
        m_Camera->SetPosition(curPos.x - movementSpeed, curPos.y, curPos.z);
        m_Camera->Render();
    }

    if (Input->IsRIGHTArrowPressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Camera->GetPosition();
        m_Camera->SetPosition(curPos.x + movementSpeed, curPos.y, curPos.z);
        m_Camera->Render();
    }
}
#include <iostream>
void ApplicationClass::ReadFileLocationsFromFile(std::vector<std::string>& fileNames, const std::string& filelistName)
{
    int filelistLenght;
    std::ifstream fin;
    std::string line;

    fin.open(filelistName);

    //std::cerr << "WTF";
    if (!fin)
    {
        return;
    }

    while (fin >> line)
    {
        fileNames.push_back(line);

        line = "";
    }

    fin.close();

    return;
}