#pragma once

//includes
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;

//class includes
#include "textureclass.h"

class SpriteClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	SpriteClass();
	SpriteClass(const SpriteClass& other);
	~SpriteClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	void Update(float frametime);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	void SetRenderLocation(int posX, int posY);
	void GetRenderLocation(int& posX, int& posY);
	bool DoJump(const double& Elapsed);
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTextures();

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_renderX, m_renderY, m_prevPosX, m_prevPosY;
	TextureClass* m_Textures;
	float m_frameTime, m_cycleTime;
	int m_currentTexture, m_textureCount;
};