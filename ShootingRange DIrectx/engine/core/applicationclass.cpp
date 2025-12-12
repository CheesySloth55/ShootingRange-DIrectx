#include "applicationclass.h"

void CenterMouse(HWND& hwnd, int screenWidth, int screenHeight);

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    std::vector<std::string> modelFilenames;
    std::vector<std::string> textureFilenames;
    std::string renderString;
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

    // camera points by default at the z direction
    m_Camera->Initialize(m_screenWidth, m_screenHeight);
    CenterMouse(m_hwnd, m_screenWidth, m_screenHeight);

    m_baseViewMatrix = m_Camera->GetViewMatrix();

    modelFilenames.push_back("engine/data/model-data/shooting_range.mesh");
    modelFilenames.push_back("engine/data/model-data/cube.mesh");


    //initialize meshes
    m_MeshClass = std::make_unique<MeshClass>();

    result = m_MeshClass->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[0], textureFilenames);
    if (!result)
    {
        return false;
    }

    m_RenderTexture = std::make_unique<RenderTextureClass>();

    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), 1080, 1080, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

    m_DisplayPlane = std::make_unique<DisplayPlaneClass>();

    result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), 3.0f, 3.0f);
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
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDirection(1.0f, 0.0f, 0.5f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(16.0f);

    m_Font = std::make_unique<FontClass>();

    result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
    if (!result)
    {
        return false;
    }

    renderString = "Render Count: 0";

    // Create and initialize the text object for the render count string.
    m_RenderCountString = std::make_unique<TextClass>();

    result = m_RenderCountString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font.get(), renderString.c_str(), 10, 10, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    m_ShaderManager = std::make_unique<ShaderManagerClass>();

    result = m_ShaderManager->Intialize(m_Direct3D->GetDevice(), m_hwnd);
    if (!result)
    {
        return false;
    }

    m_Position = std::make_unique<PositionClass>();

    m_Frustum = std::make_unique<FrustumClass>();
    // Pseudocode plan:
    // 1. Store the jthread as a member variable to keep it alive for the application's lifetime.
    // 2. In Initialize(), start the jthread and have it repeatedly call Render() in a loop.
    // 3. Use a stop_token to allow the thread to exit cleanly when requested (e.g., on shutdown).
    // 4. Ensure thread safety if Render() or related resources are accessed from multiple threads.

    // Step 1: Add a member variable to ApplicationClass (in the header):
    // std::jthread m_renderThread;

    // Step 2: Start the thread in Initialize():

    
    // Note: You must include <thread> and <chrono> headers, and ensure Render() is thread-safe.
    return true;
}


void ApplicationClass::Shutdown()
{


    // Release the model list object.
    if (m_ModelList)
    {
        m_ModelList->Shutdown();
    }

    // Release the text objects for the render count string.
    if (m_RenderCountString)
    {
        m_RenderCountString->Shutdown();
    }

    // Release the font object.
    if (m_Font)
    {
        m_Font->Shutdown();
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

    m_Camera->Update(frameTime);

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
        physicsTimeAccumulator -= physicsFrametime;
    }

    return true;
}

bool ApplicationClass::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotationMatrix;
    bool result;
    XMFLOAT3 curPos;

    m_Direct3D->BeginScene(0.2f, 0.2f, 1.0f, 1.0f);

    // Set the position of the camera for viewing the display planes with the render textures on them.

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    viewMatrix = m_Camera->GetViewMatrix();
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

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
        m_Camera->MoveForward();
    }

    if (Input.IsDOWNArrowPressed() || Input.IsSKeyPressed())
    {
        m_Camera->MoveBackward();
    }

    if (Input.IsLEFTArrowPressed() || Input.IsAKeyPressed())
    {
        m_Camera->MoveLeft();
    }

    if (Input.IsRIGHTArrowPressed() || Input.IsDKeyPressed())
    {
        m_Camera->MoveRight();
    }

    if (Input.IsSpacePressed())
    {
        m_Camera->MoveUp(deltaTime);
    }
    if (Input.IsControlPressed())
    {
        m_Camera->MoveDown(deltaTime);
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

    m_Camera->HandleMouseMovement(mouseX, mouseY);

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