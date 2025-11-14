#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{
    m_LightShader = NULL;
    m_NormalMapShader = NULL;
    m_MultiTextureShader = NULL;
    m_FontShader = NULL;
    m_AlphaMapShader = NULL;
    m_SpecMapShader = NULL;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Intialize(ID3D11Device* device, HWND hwnd)
{
    bool result;

    m_MultiTextureShader = new MultiTextureShaderClass;

    result = m_MultiTextureShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    // Create and initialize the light shader object.
    m_LightShader = new LightShaderClass;

    result = m_LightShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    m_NormalMapShader = new NormalMapShaderClass;

    result = m_NormalMapShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    m_FontShader = new FontShaderClass;

    result = m_FontShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    m_AlphaMapShader = new AlphaMapShaderClass;

    result = m_AlphaMapShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    m_SpecMapShader = new SpecMapShaderClass;

    result = m_SpecMapShader->Initialize(device, hwnd);
    if (!result)
    {
        return false;
    }

    return true;
}


void ShaderManagerClass::Shutdown()
{
    // Release the normal map shader object.
    if (m_NormalMapShader)
    {
        m_NormalMapShader->Shutdown();
        delete m_NormalMapShader;
        m_NormalMapShader = 0;
    }

    // Release the light shader object.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // Release the texture shader object.
    if (m_MultiTextureShader)
    {
        m_MultiTextureShader->Shutdown();
        delete m_MultiTextureShader;
        m_MultiTextureShader = 0;
    }

    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    if (m_AlphaMapShader)
    {
        m_AlphaMapShader->Shutdown();
        delete m_AlphaMapShader;
        m_AlphaMapShader = 0;
    }

    if (m_SpecMapShader)
    {
        m_SpecMapShader->Shutdown();
        delete m_SpecMapShader;
        m_SpecMapShader = 0;
    }

    return;
}

bool ShaderManagerClass::RenderMultiTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuseColor)
{
    bool result;


    result = m_MultiTextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, lightDirection, ambient, diffuseColor);
    if (!result)
    {
        return false;
    }

    return true;
}


bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuseColor)
{
    bool result;


    result = m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambient,diffuseColor);
    if (!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderNormalMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
    bool result;


    result = m_NormalMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuseColor);
    if (!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
    bool result;


    result = m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
    if (!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderAlphaMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3)
{
    bool result;


    result = m_AlphaMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, texture3);
    if (!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderSpecMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, 
    XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
    bool result;


    result = m_SpecMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, texture3, lightDirection, diffuseColor, cameraPosition, specularColor, specularPower );
    if (!result)
    {
        return false;
    }

    return true;
}