#pragma once

//includes
#include <memory>

//class includes (shaderclasses)
#include "shaders/lightshaderclass.h"
#include "shaders/normalmapshaderclass.h"
#include "shaders/multitextureshaderclass.h"
#include "shaders/fontshaderclass.h"
#include "shaders/alphamapshaderclass.h"
#include "shaders/textureshaderclass.h"
#include "shaders/specmapshaderclass.h"



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
	bool RenderMultiTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
	bool RenderAlphaMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);
	bool RenderSpecMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT3, XMFLOAT4, float);
	bool RenderFontShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:

	std::unique_ptr<LightShaderClass> m_LightShader;
	std::unique_ptr<NormalMapShaderClass> m_NormalMapShader;
	std::unique_ptr<MultiTextureShaderClass> m_MultiTextureShader;
	std::unique_ptr<FontShaderClass> m_FontShader;
	std::unique_ptr<AlphaMapShaderClass> m_AlphaMapShader;
	std::unique_ptr<SpecMapShaderClass> m_SpecMapShader;
	std::unique_ptr<TextureShaderClass> m_TextureShader;

};	