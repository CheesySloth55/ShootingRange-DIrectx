#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
    m_TextureShader = 0;
    m_Bitmap = 0;
    m_Sprite = 0;
    m_Timer = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char bitmapFilename[128];
    char textureFilename[128];
    bool result;
    char modelFilename[128];
    char videoCardName[128];
    int videoCardMemory;

    // Create and initialize the Direct3D object.
    m_Direct3D = new D3DClass;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    m_Direct3D->GetVideoCardInfo(videoCardName, videoCardMemory);

    std::ofstream fout;
    fout.open("videoCardinfo.txt");

    fout << "Name: " << videoCardName << '\n';
    fout << "Memory: +" << videoCardMemory << '\n';
    // Create the camera object.
    m_Camera = new CameraClass;
    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();

    m_TextureShader = new TextureShaderClass;
    
    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }   

    // Set the file name of the bitmap file.
    strcpy_s(bitmapFilename, "stone01.tga");

    // Create and initialize the bitmap object.
    m_Bitmap = new BitmapClass;

    result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 
        screenWidth, screenHeight, bitmapFilename, 1000, 500);
    if (!result)
    {
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the bitmap object.
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

    // Release the texture shader object.
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
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


    // Render the graphics scene.
    result = Render();
    if (!result)
    {
        return false;
    }

    return true;
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
    bool result;


    // Clear the buffers to begin the scene.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    // Turn off the Z buffer to begin all 2D rendering.
    m_Direct3D->TurnZBufferOff();

    // Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
    result = m_Bitmap->Render(m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }

    // Render the bitmap with the texture shader.
    result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
    if (!result)
    {
        return false;
    }

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_Direct3D->TurnZBufferOn();

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}