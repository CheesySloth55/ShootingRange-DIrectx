#include "applicationclass.h"

void CenterMouse(HWND& hwnd, int screenWidth, int screenHeight);



bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    std::vector<std::string> modelFilenames;
    std::vector<std::string> textureFilenames;
    std::string textString;
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
    m_Camera->GetUIMatrix(m_baseViewMatrix); //for text
    // camera points by default at the z direction
    CenterMouse(m_hwnd, m_screenWidth, m_screenHeight);

    modelFilenames.push_back("engine/data/model-data/spartan.mesh");
    modelFilenames.push_back("engine/data/model-data/supershotgun.mesh");
    modelFilenames.push_back("engine/data/model-data/shooting_range.mesh");
    modelFilenames.push_back("engine/data/model-data/bullet.mesh");

    m_MeshClasses.resize(modelFilenames.size());

    //initialize meshes
    for (int i{}; i < m_MeshClasses.size(); ++i)
    {
        m_MeshClasses[i] = std::make_unique<MeshClass>();
        result = m_MeshClasses[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[i], textureFilenames);
        if (!result)
        {
            MessageBox(m_hwnd, L"Could not initialize MeshClass :" + i + 1, L"Error", MB_OK);
            return false;
        }
    }

    //initialize timer class;
    m_Timer = std::make_unique<TimerClass>();

    result = m_Timer->Initialize();
    if(!result)
    {
        MessageBox(m_hwnd, L"Could not initialize Timer", L"Error", MB_OK);
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
        MessageBoxW(hwnd, L"Failed to initialize shaders in shader manager class", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the font object.
    m_Font = std::make_unique<FontClass>();

    result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
    if (!result)
    {
        return false;
    }

    textString = "Shooting range - Adam van der velden";

    m_TextString = std::make_unique<TextClass>();

    result = m_TextString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 128, m_Font.get(), textString.c_str(), 10, 10, 0.0f, 1.0f, 0.0f);
    if (!result)
    {
        return false;
    }


    return true;
}


void ApplicationClass::Shutdown()
{

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
        deltaTime = physicsTimeAccumulator;
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

    XMFLOAT3 cameraPos = m_Camera->GetPosition();
    XMVECTOR angle = m_Camera->GetRotationQuat();
   
    //person
    result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_MeshClasses[0]->GetTexture(0),
        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }
    m_MeshClasses[0]->Render(m_Direct3D->GetDeviceContext());

    // shooting range
    result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_MeshClasses[0]->GetTexture(0),
        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }
    m_MeshClasses[2]->Render(m_Direct3D->GetDeviceContext());

    //gun 
    m_Direct3D->GetWorldMatrix(worldMatrix);

    float s = 0.05f;
    XMMATRIX scale = XMMatrixScaling(s, s, s);
    XMVECTOR fixQuat = XMQuaternionRotationAxis(
        XMVectorSet(0, 1, 0, 0),
        XMConvertToRadians(90.0f)
    );
    XMVECTOR finalQuat = XMQuaternionMultiply(fixQuat, angle);
    XMMATRIX rotationm = XMMatrixRotationQuaternion(finalQuat);
    XMMATRIX translation = XMMatrixTranslation(cameraPos.x-1, cameraPos.y - 0.5f, cameraPos.z + 0.2f);
    worldMatrix = scale * rotationm * translation;
    result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_MeshClasses[0]->GetTexture(0),
        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }
    m_MeshClasses[1]->Render(m_Direct3D->GetDeviceContext());

    //bullets?
    m_Direct3D->GetWorldMatrix(worldMatrix);

    if (m_bulletCount > m_bullets.size())
    {
        Bullet b;

        XMFLOAT3 camPos = m_Camera->GetPosition();
        XMVECTOR camQuat = m_Camera->GetRotationQuat();

        // Forward vector in camera space (DirectX: +Z is forward)
        XMVECTOR localForward = XMVectorSet(0, 0, 1, 0);

        // Rotate it into world space
        XMVECTOR worldForward = XMVector3Rotate(localForward, camQuat);

        b.position = XMLoadFloat3(&camPos) + worldForward * 1.0f;  // spawn in front of camera
        b.direction = XMVector3Normalize(worldForward);
        b.speed = 50.00f;   // units per second

        m_bullets.push_back(b);
    }


    for (Bullet& b : m_bullets)
    {
        b.position += b.direction * b.speed * deltaTime;
        b.life += deltaTime;
    }

    std::vector<Bullet> newBullets;
    for (Bullet& b : m_bullets)
    {
        float maxDistance = 1.0f;
        if (b.life < maxDistance / b.speed)
            newBullets.push_back(b);
    }
    m_bullets = newBullets;
    
    scale = XMMatrixScaling(s, s, s);
    
    worldMatrix = scale * rotationm * translation;

    for (Bullet& b : m_bullets)
    {
        XMMATRIX translation = XMMatrixTranslationFromVector(b.position);

        // Optional: orient bullet along its travel direction
        XMMATRIX rotation = XMMatrixLookToLH(
            XMVectorZero(),
            b.direction,
            XMVectorSet(0, 1, 0, 0)
        );

        XMMATRIX world = scale * rotation * translation;
        result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), world, viewMatrix, projectionMatrix, m_MeshClasses[0]->GetTexture(0),
            m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
        if (!result)
        {
            return false;
        }
        m_MeshClasses[3]->Render(m_Direct3D->GetDeviceContext());
    }


    ///UI rendering and text
    m_Direct3D->TurnZBufferOff();
    m_Direct3D->EnableAlphaBlending();

    m_Direct3D->GetWorldMatrix(worldMatrix);

    result = m_ShaderManager->RenderFontShader(m_Direct3D->GetDeviceContext(), worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), m_TextString->GetPixelColor());
    m_TextString->Render(m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOn();
    m_Direct3D->DisableAlphaBlending();

    m_Direct3D->EndScene();

    return true;
}

void ApplicationClass::HandleKeyboardInput(InputClass& Input, float deltaTime)
{
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

    if (Input.IsLeftMousePressed())
    {
       ++m_bulletCount;
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