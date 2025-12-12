////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>
#include <wrl/client.h>
#include <memory>
using namespace Microsoft::WRL;
////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass() = default;
	TextureClass(const TextureClass&) = delete;
	~TextureClass() = default;

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*,const char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
    int GetHeight();

private:
	bool LoadTarga32Bit(const char*);

private:
	std::unique_ptr<unsigned char[]> m_targaData;
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_textureView;
	int m_width, m_height;
};

#endif