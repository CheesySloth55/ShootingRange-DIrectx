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
    char spriteFilename1[128];
    char spriteFilename2[128];


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

    // Create and initialize the texture shader object.
    m_TextureShader = new TextureShaderClass;

    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }
    // Set the sprite info file we will be using.
    strcpy_s(spriteFilename1, "../Engine/data/sprite_data_01.txt");
    strcpy_s(spriteFilename2, "../Engine/data/sprite_data_02.txt");
    // Create and initialize the sprite object.
    m_Sprite = new SpriteClass;
    m_SpriteJump = new SpriteClass;

    result = m_Sprite->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, spriteFilename1, 50, 50);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize sprite1", L"Error", MB_OK);
        return false;
    }

    result = m_SpriteJump->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, spriteFilename2, 50, 50);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize sprite2", L"Error", MB_OK);
        return false;
    }

    m_Sprite->SetRenderLocation(200, 400);
    m_SpriteJump->SetRenderLocation(200, 400);
    // Create and initialize the timer object.
    m_Timer = new TimerClass;

    result = m_Timer->Initialize();
    if (!result)
    {
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the timer object.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Release the sprite object.
    if (m_Sprite)
    {
        m_Sprite->Shutdown();
        delete m_Sprite;
        m_Sprite = 0;
    }

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
    float frameTime;
    static float rotation = 0.0f;
    bool result;
    int posX, posY;
    static float elapsed;

    m_SpriteJump->GetRenderLocation(posX, posY);

    // Update the system stats.
    m_Timer->Frame();

    // Get the current frame time.
    frameTime = m_Timer->GetTime();

    elapsed += frameTime;
    // Update the sprite object using the frame time.
    if (posX >= 600 && posX <= 950)
    {
        
        m_jump = true;
    }
    else
    {
        elapsed = 0;
        m_jump = false;
    }
    

    m_Sprite->Update(frameTime, elapsed);
    m_SpriteJump->Update(frameTime, elapsed);

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
    m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    // Turn off the Z buffer to begin all 2D rendering.
    m_Direct3D->TurnZBufferOff();

    // Put the sprite vertex and index buffers on the graphics pipeline to prepare them for drawing.
    if (!m_jump)
    {
        result = m_Sprite->Render(m_Direct3D->GetDeviceContext());
        if (!result)
        {
            return false;
        }

        result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Sprite->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Sprite->GetTexture());
        if (!result)
        {
            return false;
        }
    }
    else
    {
        result = m_SpriteJump->Render(m_Direct3D->GetDeviceContext());
        if (!result)
        {
            return false;
        }

        result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SpriteJump->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_SpriteJump->GetTexture());
        if (!result)
        {
            return false;
        }
    }

    // Render the sprite with the texture shader.

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_Direct3D->TurnZBufferOn();

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}