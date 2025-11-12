
#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
    m_SpecMapShader = NULL;
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

    // camera points by default at the z direction
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
    m_Camera->Render();

    // Create and initialize the specular map shader object.
    m_SpecMapShader = new SpecMapShaderClass;

    result = m_SpecMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
        return false;
    }

    //model data
    strcpy_s(modelFilename, "engine/data/model-data/sphere.txt");

    //textures
    strcpy_s(textureFilename1, "engine/data/images/stone02.tga");
    strcpy_s(textureFilename2, "engine/data/images/normal02.tga");
    strcpy_s(textureFilename3, "engine/data/images/spec02.tga");


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

    // Create and initialize the light object.
    m_Light = new LightClass;

    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(5.0f);

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

    if (m_SpecMapShader)
    {
        m_SpecMapShader->Shutdown();
        delete m_SpecMapShader;
        m_SpecMapShader = 0;
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
    float frameTime;
    static float rotation = 360.0f;

    m_Timer->Frame();

    // Get the current frame time.
    frameTime = m_Timer->GetFrameTime();

    rotation -= 0.0174532925f * 0.10f;
    if (rotation <= 0.0f)
    {
        rotation += 360.0f;
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();


    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // Rotate the world matrix by the rotation value so that the model will spin.
    worldMatrix = XMMatrixRotationY(rotation);

    // Render the model using the multitexture shader.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    result = m_SpecMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0), m_Model->GetTexture(1), m_Model->GetTexture(2), m_Light->GetDirection(), m_Light->GetDiffuseColor(),
        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
    if (!result)
    {
        return false;
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