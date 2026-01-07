#include "applicationclass.h"

void CenterMouse(HWND& hwnd, int screenWidth, int screenHeight);

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    std::vector<std::string> modelFilenames;
    std::vector<std::string> textureFilenames;
    std::string ammoCount;
    const std::string texturelistLocation{"engine/data/texturelist.txt"};
    bool result;

    ReadFileLocationsFromFile(textureFilenames, texturelistLocation);

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    m_hwnd = hwnd;
    // Create and initialize the Direct3D object.
    m_Direct3D = std::make_unique<D3DClass>();
    
    result = m_Direct3D->Initialize(m_screenWidth, m_screenHeight, true, m_hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(m_hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the camera object.
    m_Camera = std::make_unique<CameraClass>();
    m_Camera->Render();
    // camera points by default at the z direction
    CenterMouse(m_hwnd, m_screenWidth, m_screenHeight);

    m_Camera->GetViewMatrix(m_baseViewMatrix);

    modelFilenames.push_back("engine/data/model-data/shooting_range.mesh");
    modelFilenames.push_back("engine/data/model-data/cube.mesh");


    //initialize meshes
    m_MeshClass = std::make_unique<MeshClass>();

    result = m_MeshClass->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[0], textureFilenames);
    if (!result)
    {
        return false;
    }

    //initialize timer class;
    m_Timer = std::make_unique<TimerClass>();

    result = m_Timer->Initialize();
    if(!result)
    {
        return false;
    }

    // Create and initialize the light object.
    m_Light = std::make_unique<LightClass>();

    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
    m_Light->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
    m_Light->SetDirection(-1.0f, -1.0f, 0.25f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(16.0f);

    m_ShaderManager = std::make_unique<ShaderManagerClass>();

    result = m_ShaderManager->Intialize(m_Direct3D->GetDevice(), m_hwnd);
    if (!result)
    {
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{


    // Release the model list object.
    if (m_ModelList)
    {
        m_ModelList->Shutdown();
    }

    // Release the font shader object.
    if (m_ShaderManager)
    {
        m_ShaderManager->Shutdown();
    }

    // Release the Direct3D object.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
    }

    return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
    bool result ,keyDown;
    float rotationY;
    static float rotation = 360.0f;
    static float physicsTimeAccumulator{};
    static float physicsFrametime = 1.0f/ 60.0f;
    float frameTime;

    m_Timer->Frame();

    frameTime = m_Timer->GetFrameTime();
    physicsTimeAccumulator += frameTime;

    m_Camera->Render();
    while (physicsTimeAccumulator >= physicsFrametime)
    {
        rotation -= 0.0174532925f * 0.5f; // 1 degree in radians
        if (rotation <= 0.0f)
        {
            rotation += 360.0f;
        }

        // Check if the user pressed escape and wants to exit the application.
        if (Input->IsEscapePressed())
        {
            return false;
        }

        HandleMouseMovement(*Input, physicsTimeAccumulator);
        HandleKeyboardInput(*Input, physicsTimeAccumulator);
        m_Camera->UpdatePhysics(physicsTimeAccumulator);
        physicsTimeAccumulator -= physicsFrametime;
    }

    return true;
}

bool ApplicationClass::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, rotationMatrix;
    bool result;
    XMFLOAT3 curPos;

    m_Direct3D->BeginScene(0.2f, 0.2f, 1.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);
    rotationMatrix = XMMatrixRotationY(0.0174532925f * 90);
    // Render the display plane using the texture shader and the render texture resource.
    result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), rotationMatrix, viewMatrix, projectionMatrix, m_MeshClass->GetTexture(0),
        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }
    m_MeshClass->Render(m_Direct3D->GetDeviceContext());

    m_Direct3D->EndScene();

    return true;
}

void ApplicationClass::HandleKeyboardInput(InputClass& Input, float deltaTime)
{

    static float movementSpeed = 0.15f;
    if (Input.IsF11Pressed())
    {
        m_Direct3D->ToggleFullScreenMode();
    }
    
    if (Input.IsUPArrowPressed() || Input.IsWKeyPressed())
    {
        m_Camera->MoveForward(deltaTime);
    }

    if (Input.IsDOWNArrowPressed() || Input.IsSKeyPressed())
    {
        m_Camera->MoveBackward(deltaTime);
    }

    if (Input.IsLEFTArrowPressed() || Input.IsAKeyPressed())
    {
        m_Camera->StrafeLeft(deltaTime);
    }

    if (Input.IsRIGHTArrowPressed() || Input.IsDKeyPressed())
    {
        m_Camera->StrafeRight(deltaTime);
    }

    if (Input.IsSpacePressed())
    {
        m_Camera->Jump();
    }
}

void ApplicationClass::HandleMouseMovement(InputClass& Input, float deltaTime)
{
    if (GetForegroundWindow() != m_hwnd)
    {
        return;
    }

    int mouseX;
    int mouseY;
    Input.GetMouseLocation(mouseX, mouseY);

    m_Camera->ProcessMouseInput(mouseX, mouseY);

    CenterMouse(m_hwnd, m_screenWidth, m_screenHeight);

    return;
}

void ApplicationClass::ReadFileLocationsFromFile(std::vector<std::string>& fileNames, const std::string& filelistName)
{
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

void CenterMouse(HWND& hwnd, int screenWidth, int screenHeight)
{
    POINT center;
    center.x = screenWidth / 2;
    center.y = screenHeight / 2;

    ClientToScreen(hwnd, &center);
    SetCursorPos(center.x, center.y);
}