#pragma once

//includes
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
using namespace DirectX;
using namespace Microsoft::WRL;


class RenderTextureClass
{
public:
	RenderTextureClass() = default;
	RenderTextureClass(const RenderTextureClass& other) = delete;
	~RenderTextureClass() = default;

	bool Initialize(ID3D11Device*, int, int, float, float, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void GetProjectionMatrix(XMMATRIX& matrix);
	void GetOrthoMatrix(XMMATRIX& matrix);

	int	GetTextureWidth();
	int GetTextureHeight();

private:
	int m_textureWidth{};
	int m_textureHeight{};
	ComPtr<ID3D11Texture2D> m_renderTargetTexture;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;

};