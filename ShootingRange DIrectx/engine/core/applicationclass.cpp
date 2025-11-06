
#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
    m_AlphaMapShader = NULL;
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
    char modelFilename[128], textureFilename1[128], textureFilename2[128], textureFilename3[128];
    bool result;


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

    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
    m_Camera->Render();
    // Create and initialize the alpha map shader object.
    m_AlphaMapShader = new AlphaMapShaderClass;

    result = m_AlphaMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the alpha map shader object.", L"Error", MB_OK);
        return false;
    }

    // Set the file name of the model.
    strcpy_s(modelFilename, "engine/data/model-data/square.txt");

    strcpy_s(textureFilename1, "engine/data/images/stone01.tga");
    strcpy_s(textureFilename2, "engine/data/images/dirt01.tga");
    strcpy_s(textureFilename3, "engine/data/images/alpha01.tga");

    // Create and initialize the model object.
    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, textureFilename3);
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

    // Release the alpha map shader object.
    if (m_AlphaMapShader)
    {
        m_AlphaMapShader->Shutdown();
        delete m_AlphaMapShader;
        m_AlphaMapShader = 0;
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
    float frameTime;
    m_Timer->Frame();

    // Get the current frame time.
    frameTime = m_Timer->GetFrameTime();

    // Check if the user pressed escape and wants to exit the application.
    if (Input->IsEscapePressed())
    {
        return false;
    }

    if (Input->IsF11Pressed())
    {
        m_Direct3D->ToggleFullScreenMode();
    }


    result = Render();
    if (!result)
    {
        return false;
    }

    return true;
}



bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();


    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // Render the model using the multitexture shader.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    result = m_AlphaMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0), m_Model->GetTexture(1), m_Model->GetTexture(2));
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}

