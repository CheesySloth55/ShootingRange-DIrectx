#pragma once

//class includes (shaderclasses)
#include "shaders/lightshaderclass.h"
#include "shaders/normalmapshaderclass.h"
#include "shaders/multitextureshaderclass.h"

class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Intialize(ID3D11Device*, HWND);
	void Shutdown();

	bool RenderLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
	bool RenderNormalMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,XMFLOAT3, XMFLOAT4);
	bool RenderMultiTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

private:

	LightShaderClass* m_LightShader;
	NormalMapShaderClass* m_NormalMapShader;
	MultiTextureShaderClass* m_MultiTextureShader;
};	