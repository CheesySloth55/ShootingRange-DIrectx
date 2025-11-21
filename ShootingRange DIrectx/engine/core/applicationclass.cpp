
#include "applicationclass.h"
#include <vector>
#include <string>
#include <fstream>

#define MAX_MODELCOUNT 1000
ApplicationClass::ApplicationClass()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
    m_ShaderManager = NULL;
    m_Light = NULL;
    m_Timer = NULL;
    m_Font = NULL;
    m_RenderCountString = NULL;
    m_ModelList = NULL;
    m_Position = NULL;
    m_Frustum = NULL;
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
    std::string renderString;
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
    m_Camera->SetPosition(0.0f, 0.0f, -100.0f);
    m_Camera->Render();
    m_Camera->GetViewMatrix(m_baseViewMatrix);

    //model data
    modelFilenames.push_back("engine/data/model-data/cube.txt");
    modelFilenames.push_back("engine/data/model-data/sphere.txt");
    modelFilenames.push_back("engine/data/model-data/square.txt");

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
    m_Light->SetDirection(-1.0f, -0.8f, 0.3f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(16.0f);

    m_Font = new FontClass;

    result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
    if (!result)
    {
        return false;
    }

    renderString = "Render Count: 0";

    // Create and initialize the text object for the render count string.
    m_RenderCountString = new TextClass;

    result = m_RenderCountString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, renderString.c_str() , 10, 10, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    m_ShaderManager = new ShaderManagerClass;

    result = m_ShaderManager->Intialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }

    m_ModelList = new ModelListClass;
    m_ModelList->Initialize(MAX_MODELCOUNT);

    m_Position = new PositionClass;

    m_Frustum = new FrustumClass;

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the model object.
    if (m_Frustum)
    {
        delete m_Frustum;
        m_Frustum = 0;
    }

    // Release the position object.
    if (m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // Release the timer object.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Release the model list object.
    if (m_ModelList)
    {
        m_ModelList->Shutdown();
        delete m_ModelList;
        m_ModelList = 0;
    }

    // Release the text objects for the render count string.
    if (m_RenderCountString)
    {
        m_RenderCountString->Shutdown();
        delete m_RenderCountString;
        m_RenderCountString = 0;
    }

    // Release the font object.
    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = 0;
    }

    // Release the font shader object.
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

    // Release the model object.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
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
    bool result ,keyDown;
    float rotationY;
    static float rotation = 360.0f;
    static float physicsTimeAccumulator{};
    static float physicsFrametime = 0.01;
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



    m_Position->GetRotation(rotationY);

    m_Camera->SetRotation(0.0f, rotationY, 0.0f);
    m_Camera->Render();

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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, rotationMatrix;
    float positionX, positionY, positionZ, radius;
    int modelCount, renderCount, i;
    bool renderModel, result;

    m_Direct3D->BeginScene(0.5f, 0.8f, 1.0f, 1.0f);

    m_Camera->Render();

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

    // Render the model using the normal map shader.
    modelCount = m_ModelList->GetModelCount();

    renderCount = 0;

    rotationMatrix = XMMatrixRotationY(rotation);

    for (i = 0; i < modelCount; i++)
    {

        // Get the position and color of the sphere model at this index.
        m_ModelList->GetData(i, positionX, positionY, positionZ);

        // Set the radius of the sphere to 1.0 since this is already known.
        radius = 1.0f;

        // Check if the sphere model is in the view frustum.
        renderModel = m_Frustum->CheckCube(positionX, positionY, positionZ, radius);

        // If it can be seen then render it, if not skip this model and check the next sphere.
        if (renderModel)
        {
            // Move the model to the location it should be rendered at.
            worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
            worldMatrix = XMMatrixMultiply(rotationMatrix, worldMatrix);
            // Render the model using the light shader.
            m_Model->Render(m_Direct3D->GetDeviceContext());

            //result = m_ShaderManager->RenderSpecMapShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
            //    m_Model->GetTexture(3), m_Model->GetTexture(4), m_Model->GetTexture(5), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
            //if (!result)
            //{
            //    return false;
            //}

            result = m_ShaderManager->RenderMultiTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
                m_Model->GetTexture(0), m_Model->GetTexture(2), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
            if (!result)
            {
                return false;
            }

            // Since this model was rendered then increase the count for this frame.
            renderCount++;
        }
    }

    result = UpdateRenderCountString(renderCount);
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOff();
    m_Direct3D->EnableAlphaBlending();

    // Reset the world matrix.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // Render the render count text string using the font shader.
    m_RenderCountString->Render(m_Direct3D->GetDeviceContext());

    result = m_ShaderManager->RenderFontShader(m_Direct3D->GetDeviceContext(), m_RenderCountString->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix,
        m_Font->GetTexture(), m_RenderCountString->GetPixelColor());
    if (!result)
    {
        return false;
    }

    // Enable the Z buffer and disable alpha blending now that 2D rendering is complete.
    m_Direct3D->TurnZBufferOn();
    m_Direct3D->DisableAlphaBlending();

    m_Direct3D->EndScene();

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
    result = m_RenderCountString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 0.0f, 0.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    return true;
}

void ApplicationClass::HandleKeyboardInput(InputClass* Input)
{
    bool keyDown{};
    static float lightchangeSpeed = 0.005;

    if (Input->IsF11Pressed())
    {
        m_Direct3D->ToggleFullScreenMode();
    }
    //WASD = movement relative to you
    //TODO: handle Mouse movement
    keyDown = Input->IsLEFTArrowPressed();
    m_Position->TurnLeft(keyDown);

    keyDown = Input->IsRIGHTArrowPressed();
    m_Position->TurnRight(keyDown);

    keyDown = Input->IsDOWNArrowPressed();
    m_Position->TurnLeft(keyDown);

    keyDown = Input->IsUPArrowPressed();
    m_Position->TurnRight(keyDown);

    if (Input->Is1Pressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Light->GetDirection();
        m_Light->SetDirection(curPos.x , curPos.y, curPos.z + lightchangeSpeed);

    }

    if (Input->Is2Pressed())
    {
        XMFLOAT3 curPos;
        curPos = m_Light->GetDirection();
        m_Light->SetDirection(curPos.x , curPos.y, curPos.z - lightchangeSpeed);
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