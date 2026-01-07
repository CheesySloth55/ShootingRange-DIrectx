#pragma once
#include <DirectXMath.h>

struct Vertex2D
{
    DirectX::XMFLOAT3 position;   // XY position on screen, Z for depth (0 for UI)
    DirectX::XMFLOAT2 tex;        // UV coordinates for the font/texture
    DirectX::XMFLOAT4 color;      // RGBA color
};
