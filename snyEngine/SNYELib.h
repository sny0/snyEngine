#pragma once
#include<DirectXMath.h>

using namespace DirectX;

//#pragma pack(1)
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
};
//#pragma pack()
struct SceneData {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
	XMFLOAT3 eye;
};

struct Material {
	XMFLOAT3 diffuse;
	XMFLOAT3 specular;
	XMFLOAT3 ambient;
};