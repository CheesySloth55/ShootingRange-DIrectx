#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class FrustumClass
{
public:
	FrustumClass();
	FrustumClass(const FrustumClass& other);
	~FrustumClass();

	void ConstructFrustum(XMMATRIX, XMMATRIX, float);

	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float x, float y, float z, float radius);
	bool CheckSphere(float x, float y, float z, float radius);
	bool CheckRectangle(float x, float y, float z, float xSize, float ySize, float zSize);

private:
	XMFLOAT4 m_planes[6];
};