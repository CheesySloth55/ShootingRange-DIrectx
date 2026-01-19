#include "textclass.h"


TextClass::TextClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, int screenWidth, int screenHeight, int maxLength, FontClass * Font,const char* text,
    int positionX, int positionY, float red, float green, float blue)
{
    bool result;


    // Store the screen width and height.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Store the maximum length of the sentence.
    m_maxLength = maxLength;

    // Initalize the sentence.
    result = InitializeBuffers(device, deviceContext, Font, text, positionX, positionY, red, green, blue);
    if (!result)
    {
        return false;
    }

    return true;
}

void TextClass::Shutdown()
{
    // Release the vertex and index buffers.
    ShutdownBuffers();

    return;
}

void TextClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return;
}

int TextClass::GetIndexCount()
{
    return m_indexCount;
}

bool TextClass::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FontClass* Font,const char* text, int positionX, int positionY, float red, float green, float blue)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    

    m_vertexCount = 6 * m_maxLength;
    m_indexCount = m_vertexCount;
    
    vertices = new VertexType[m_vertexCount];
    indices = new unsigned long[m_indexCount];

    memset(vertices, 0, (sizeof(VertexType*) * m_vertexCount));

    for (int i{ 0 }; i < m_indexCount; i++)
    {
        indices[i] = i;
    }

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    delete[] vertices;
    vertices = NULL;

    delete[] indices;
    indices = NULL;

    result = UpdateText(deviceContext, Font, text, positionX, positionY, red, green, blue);
    if (!result)
    {
        return false;
    }

    return true;
}

void TextClass::ShutdownBuffers()
{
    // Release the index buffer.
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // Release the vertex buffer.
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

bool TextClass::UpdateText(ID3D11DeviceContext* deviceContext, FontClass* Font, const char* text, int positionX, int positionY, float red, float green, float blue)
{
    int numLetters;
    VertexType* vertices;
    float drawX, drawY;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* verticesPtr;


    m_pixelColor = XMFLOAT4(red, green, blue, 1.0f);

    numLetters = (int)strlen(text);

    if (numLetters > m_maxLength)
    {
        return false;
    }

    vertices = new VertexType[m_vertexCount];

    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
    drawY = (float)((m_screenHeight / 2) - positionY);

    Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    verticesPtr = (VertexType*)mappedResource.pData;

    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    deviceContext->Unmap(m_vertexBuffer, 0);

    delete[] vertices;
    vertices = NULL;

    return true;
}


void TextClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride, offset;


    stride = sizeof(VertexType);
    offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->DrawIndexed(m_indexCount, 0, 0);

    return;
}

XMFLOAT4 TextClass::GetPixelColor()
{
    return m_pixelColor;
}