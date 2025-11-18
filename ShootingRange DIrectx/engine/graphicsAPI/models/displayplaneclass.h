#pragma once


//class includes
#include "../directx/d3dclass.h"

//includes
#include <wrl/client.h>
using namespace Microsoft::WRL;

class DisplayPlaneClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	DisplayPlaneClass() = default;
	DisplayPlaneClass(const DisplayPlaneClass& other) = delete;
	~DisplayPlaneClass() = default;

	bool Initialize(ID3D11Device* device, float width, float height);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device, float width, float height);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer{};
	ComPtr<ID3D11Buffer> m_indexBuffer{};
	int m_vertexCount{};
	int m_indexCount{};
};