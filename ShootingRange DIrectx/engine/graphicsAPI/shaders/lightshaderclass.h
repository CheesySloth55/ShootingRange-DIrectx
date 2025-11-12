#pragma once

//Includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;

class LightShaderClass
{
private: 
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding; // to properly calculate with multiple of 16 bit structures
	};
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass& other);
	~LightShaderClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int, XMMATRIX world, XMMATRIX view, XMMATRIX projection,
		ID3D11ShaderResourceView* SRV, XMFLOAT3 LightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* className, WCHAR* projectname);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX world, XMMATRIX view, XMMATRIX projection,
		ID3D11ShaderResourceView* SRV, XMFLOAT3 LightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse);
	void RenderShader(ID3D11DeviceContext* deviceContext, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};