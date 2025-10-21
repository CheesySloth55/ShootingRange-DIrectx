
#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
    m_LightShader = NULL;
	m_Light = NULL;
    m_FontShader = NULL;
    m_Font = NULL;
    m_TextString1 = NULL;
    m_TextString2 = NULL;
    m_Fps = NULL;
    m_FpsString = NULL;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char textureFilename[128];
    bool result;
    char modelFilename[128];
    char testString1[32], testString2[32];
    char fpsString[32];

    // Create and initialize the Direct3D object.
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new CameraClass;

    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f,0.0f, -10.0f);

    m_Direct3D->GetWorldMatrix(worldMatrixTEXT);

    // Create and initialize the font shader object.
    m_FontShader = new FontShaderClass;

    result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // Create and initialize the font object.
    m_Font = new FontClass;

    result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
    if (!result)
    {
        return false;
    }

    // Set the strings we want to display.
    //strcpy_s(testString1, "MY WIFE IS SO HOT");
    // Create and initialize the first text object.
    //m_TextString1 = new TextClass;
    //result = m_TextString1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, testString1, 
    //    screenWidth / 2 - (m_Font->GetSentencePixelLength(testString1) / 2), screenHeight / 2 - (m_Font->GetFontHeight() * 1), 0.0f, 0.0f, 1.0f);
    //if (!result)
    //{
    //    return false;
    //}

    //create FPS counter
    m_Fps = new FpsClass;

    m_Fps->Initialize();

    m_previousFps = -1;
    strcpy_s(fpsString, "Fps: 0");

    m_FpsString = new TextClass;

    result = m_FpsString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, fpsString,
        10, 10, 0.0f, 1.0f, 0.0f);
    if (!result)
    {
        return false;
    }
    // Create and initialize the model object.

    strcpy_s(modelFilename, "cube.txt");
    strcpy_s(textureFilename, "stone01.tga");

    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename, modelFilename);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

        // Create and initialize the light shader object.
    m_LightShader = new LightShaderClass;

    result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }
    // Create and initialize the light object.
    m_Light = new LightClass;

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

    // Create and initialize the font shader object.
    m_FontShader = new FontShaderClass;

    result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }
    return true;
}


void ApplicationClass::Shutdown()
{

    // Release the text object for the fps string.
    if (m_FpsString)
    {
        m_FpsString->Shutdown();
        delete m_FpsString;
        m_FpsString = 0;
    }

    // Release the fps object.
    if (m_Fps)
    {
        delete m_Fps;
        m_Fps = 0;
    }

    // Release the text string objects.
    if (m_TextString2)
    {
        m_TextString2->Shutdown();
        delete m_TextString2;
        m_TextString2 = 0;
    }

    if (m_TextString1)
    {
        m_TextString1->Shutdown();
        delete m_TextString1;
        m_TextString1 = 0;
    }

    // Release the font object.
    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = 0;
    }

    // Release the font shader object.
    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    // Release the light object.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // Release the light shader object.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
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


bool ApplicationClass::Frame()
{
    static float rotation = 0.0f;
    bool result;


    // Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.4f;
    if (rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    result = UpdateFps();
    if (!result)
    {
        return false;
    }

    // Render the graphics scene.
    result = Render(rotation);
    if (!result)
    {
        return false;
    }

    return true;
}



bool ApplicationClass::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    bool result;



    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();


    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);


    worldMatrix = XMMatrixRotationY(rotation);

    m_Model->Render(m_Direct3D->GetDeviceContext());

    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
        m_Light->GetDirection(),m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // Disable the Z buffer and enable alpha blending for 2D rendering.
    m_Direct3D->TurnZBufferOff();
    m_Direct3D->EnableAlphaBlending();


    // Render the second text string using the font shader.
    m_FpsString->Render(m_Direct3D->GetDeviceContext());

    result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_FpsString->GetIndexCount(), worldMatrixTEXT, viewMatrix, orthoMatrix,
        m_Font->GetTexture(), m_FpsString->GetPixelColor());
    if (!result)
    {
        return false;
    }

    // Enable the Z buffer and disable alpha blending now that 2D rendering is complete.
    m_Direct3D->TurnZBufferOn();
    m_Direct3D->DisableAlphaBlending();

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}

bool ApplicationClass::UpdateFps()
{
    int fps;
    char tempString[16];
    char finalString[16];
    float red;
    float green;
    float blue;
    bool result;

    m_Fps->Frame();

    fps = m_Fps->GetFps();

    if (m_previousFps == fps)
    {
        return true;
    }

    m_previousFps = fps;

    if (fps > 999999)
    {
        fps = 999999;
    }

    sprintf_s(tempString, "%d", fps);

    strcpy_s(finalString, "Fps: ");
    strcat_s(finalString, tempString);

    if (fps >= 60)
    {
        red = 0.0f;
        green = 1.0f;
        blue = 0.0f;
    }

    if (fps < 60)
    {
        red = 1.0f;
        green = 1.0f;
        blue = 0.0f;
    }

    if (fps < 30)
    {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    result = m_FpsString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, red, green, blue);
    if (!result)
    {
        return false;
    }

    return true;
}