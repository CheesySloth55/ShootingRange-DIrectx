#include "meshclass.h"
#include <exception>

bool MeshClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& modelFilename, const std::vector<std::string>& textureFilenames)
{
	bool result;

	m_textureCount = textureFilenames.size();

	result = LoadMeshFile(modelFilename);
	if (!result)
	{
		return false;
	}

	//vertex buffers and indices
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the textures for this model.
	result = LoadTextures(device, deviceContext, textureFilenames);
	if (!result)
	{
		return false;
	}

	return true;
}

void MeshClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int MeshClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* MeshClass::GetTexture(int index)
{
	return m_textures.at(index)->GetTexture();
}

bool MeshClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc{};
	D3D11_BUFFER_DESC indexBufferDesc{};
	D3D11_SUBRESOURCE_DATA vertexData{};
	D3D11_SUBRESOURCE_DATA indexData{};
	HRESULT result{};

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void MeshClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// Bind buffers once

	deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw each submesh
	for (const SubMesh& sm : m_subMeshes)
	{
		int matIndex = sm.materialIndex;
		if (matIndex >= 0 && matIndex < m_textures.size())
		{
			ID3D11ShaderResourceView* texture = m_textures[matIndex].get()->GetTexture();
			deviceContext->PSSetShaderResources(0, 1, &texture);
		}

		deviceContext->DrawIndexed(sm.indexCount, sm.indexStart, 0);
	}

	return;
}

bool MeshClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<std::string>& filenames)
{
	bool result;

	// Create and initialize the texture object array.
	m_textures.resize(m_textureCount);

	for (int i{}; i < m_textureCount; i++)
	{
		m_textures.at(i) = std::make_unique<TextureClass>();
		result = m_textures[i]->Initialize(device, deviceContext, filenames[i].c_str());
		if (!result)
		{
			return false;
		}
	}

	return true;
}

bool MeshClass::LoadMeshFile(const std::string& filename)
{
	std::ifstream fin(filename, std::ios::binary | std::ios::ate);
	MeshHeader meshHeader{};
	

	if (!fin.is_open())
	{
		throw std::runtime_error("mesh file could not be opened: " + filename);
		return false;
	}

	//uint32_t fileSize = static_cast<size_t>(fin.tellg()); is this necessary?
	fin.seekg(0, std::ios::beg);

	//read mesh header structure
	fin.read(reinterpret_cast<char*>(&meshHeader), sizeof(MeshHeader));

	if (meshHeader.name != 0x4D455348) // MESH
	{
		std::cerr << "Header name does not match\n";
		return false;
	}

	if (meshHeader.version != 1)
	{
		std::cerr << "Version does not match\n";
		return false;
	}

	if (meshHeader.vertexCount == 0)
	{
		std::cerr << "no vertex count\n";
		assert(meshHeader.vertexCount != 0);
		return false;
	}

	if (meshHeader.indexCount == 0)
	{
		std::cerr << "no index count\n";
		assert(meshHeader.indexCount != 0);
		return false;
	}

	m_vertexCount = meshHeader.vertexCount;
	m_indexCount = meshHeader.indexCount;
	m_subMeshes.resize(meshHeader.subMeshCount);
	m_vertices.resize(m_vertexCount);
	m_indices.resize(m_indexCount);

	// read the indices and vertices into the vectors
	fin.read(reinterpret_cast<char*>(m_subMeshes.data()), meshHeader.subMeshCount * sizeof(SubMesh));
	fin.read(reinterpret_cast<char*>(m_vertices.data()), m_vertexCount * sizeof(VertexType));
	fin.read(reinterpret_cast<char*>(m_indices.data()), m_indexCount * sizeof(uint32_t));

	return true;
}

