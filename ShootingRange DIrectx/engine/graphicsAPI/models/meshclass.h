#pragma once

//includes
#include <string>
#include <iostream>
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
#include <wrl/client.h>
using namespace DirectX;
using namespace Microsoft::WRL;

//class includes
#include "../textures/textureclass.h"


class MeshClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct MeshHeader
	{
		uint32_t name{ 0x4D455348 };
		uint32_t version{ 1 };

		uint32_t vertexCount{};
		uint32_t indexCount{};

		uint32_t subMeshCount{};

		uint32_t reserved[16]{};
	};

	struct SubMesh
	{
		uint32_t vertexStart{};
		uint32_t vertexCount{};
		uint32_t indexStart{};
		uint32_t indexCount{};

		uint32_t materialIndex{};
		uint32_t reserved[3] = {};
		//uint32_t BoundingBoxMin;
		//uint32_t BoundingBoxMax;
	};

public:
	MeshClass() = default;
	MeshClass(const MeshClass&) = delete;
	~MeshClass() = default;

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string&, const std::vector<std::string>&);

	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(int index);

private:
	bool InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, const std::vector<std::string>&);
	bool LoadMeshFile(const std::string& fileName);

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	uint32_t m_vertexCount{};
	uint32_t m_indexCount{};	
	uint32_t m_vertexStride{}; //bytes
	std::vector<SubMesh> m_subMeshes;
	std::vector<VertexType> m_vertices;
	std::vector<uint32_t> m_indices;
	int m_textureCount{};
	std::vector<std::unique_ptr<TextureClass>> m_textures;
};
