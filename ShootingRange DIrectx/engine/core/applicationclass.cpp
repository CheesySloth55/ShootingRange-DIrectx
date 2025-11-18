
#include "applicationclass.h"



bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    std::vector<std::string> modelFilenames;
    std::vector<std::string> textureFilenames;
    std::string renderString;
    const std::string texturelistLocation{"engine/data/texturelist.txt"};
    bool result;

    ReadFileLocationsFromFile(textureFilenames, texturelistLocation);

    // Create and initialize the Direct3D object.
    m_Direct3D = std::make_unique<D3DClass>();

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the camera object.
    m_Camera = std::make_unique<CameraClass>();

    // camera points by default at the z direction
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
    m_Camera->Render();
    m_Camera->GetViewMatrix(m_baseViewMatrix);

    //model data
    modelFilenames.push_back("engine/data/model-data/sphere.txt");
    modelFilenames.push_back("engine/data/model-data/square.txt");
    modelFilenames.push_back("engine/data/model-data/cube.txt");
    modelFilenames.push_back("engine/data/model-data/m4a1.txt");

    // Create and initialize the model object.
    m_Model = std::make_unique<ModelClass>();

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[0], textureFilenames);
    if (!result)
    {
        return false;
    }

    m_Model2 = std::make_unique<ModelClass>();

    result = m_Model2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilenames[1], textureFilenames);
    if (!result)
    {
        return false;
    }

    m_RenderTexture = std::make_unique<RenderTextureClass>();

    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), 256, 256, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        return false;
    }

    m_DisplayPlane = std::make_unique<DisplayPlaneClass>();

    result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), 1.0f, 1.0f);
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

    result = m_ShaderManager->Intialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }

    m_ModelList = std::make_unique<ModelListClass>();
    m_ModelList->Initialize(maxObjectCount);

    m_Position = std::make_unique<PositionClass>();

    m_Frustum = std::make_unique<FrustumClass>();

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

    // Release the model object.
    if (m_Model)
    {
        m_Model->Shutdown();
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
    static float physicsFrametime = 0.01f;
    float frameTime;

    m_Timer->Frame();

    frameTime = m_Timer->GetFrameTime();
    physicsTimeAccumulator += frameTime;
    while (physicsTimeAccumulator >= physicsFrametime)
    {
        rotation -= 0.0174532925f * 0.5f; // 1 degree in radians
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

    m_Position->SetFrameTime(frameTime);

    keyDown = Input->IsLEFTArrowPressed();
    m_Position->TurnLeft(keyDown);

    keyDown = Input->IsRIGHTArrowPressed();
    m_Position->TurnRight(keyDown);

    m_Position->GetRotation(rotationY);

    m_Camera->SetRotation(0.0f, rotationY, 0.0f);
    m_Camera->Render();

    HandleKeyboardInput(Input);

    result = RenderSceneToTexture(rotation);
    if (!result)
    {
        return false;
    }

    result = Render();
    if (!result)
    {
        return false;
    }

    return true;
}

bool ApplicationClass::RenderSceneToTexture(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotationMatrix;
    bool result;
   
    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.5f, 1.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_RenderTexture->GetProjectionMatrix(projectionMatrix);

    rotationMatrix = XMMatrixRotationY(rotation);

    // Render the model using the texture shader.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), rotationMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0),
                                            m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    m_Model2->Render(m_Direct3D->GetDeviceContext());

    worldMatrix = XMMatrixScaling(2.1f, 2.1f, 1.0f);

    result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(0));
    if (!result)
    {
        return false;
    }
    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;
    XMFLOAT3 curPos;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Set the position of the camera for viewing the display planes with the render textures on them.
    curPos = m_Camera->GetPosition();
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixTranslation(0.0f, 1.5f, 0.0f);

    // Render the display plane using the texture shader and the render texture resource.
    m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());

    result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture->GetShaderResourceView());
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(-1.5f, -1.5f, 0.0f);

    // Render the display plane using the texture shader and the render texture resource.
    m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());

    result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture->GetShaderResourceView());
    if (!result)
    {
        return false;
    }
    worldMatrix = XMMatrixTranslation(1.5f, -1.5f, 0.0f);

    // Render the display plane using the texture shader and the render texture resource.
    m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());

    result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture->GetShaderResourceView());
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    m_Camera->SetPosition(curPos.x, curPos.y, curPos.z);
    return true;
}

bool ApplicationClass::UpdateRenderCountString(int renderCount)
{
    char tempString[16], finalString[32];
    bool result;


    // Convert the render count integer to string format.
    sprintf_s(tempString, "%d", renderCount);

    // Setup the render count string.
    strcpy_s(finalString, "Render Count: ");
    strcat_s(finalString, tempString);

    // Update the sentence vertex buffer with the new string information.
    result = m_RenderCountString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font.get(), finalString, 10, 10, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    return true;
}

void ApplicationClass::HandleKeyboardInput(InputClass* Input)
{
    static float movementSpeed = 0.15f;
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

    //if (Input->IsLEFTArrowPressed())
    //{
    //    XMFLOAT3 curPos;
    //    curPos = m_Camera->GetPosition();
    //    m_Camera->SetPosition(curPos.x - movementSpeed, curPos.y, curPos.z);
    //    m_Camera->Render();
    //}

    //if (Input->IsRIGHTArrowPressed())
    //{
    //    XMFLOAT3 curPos;
    //    curPos = m_Camera->GetPosition();
    //    m_Camera->SetPosition(curPos.x + movementSpeed, curPos.y, curPos.z);
    //    m_Camera->Render();
    //}
}
#include <iostream>
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