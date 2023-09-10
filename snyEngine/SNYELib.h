#pragma once
#include<DirectXMath.h>

using namespace DirectX;


struct Vertex {
	XMFLOAT3 position; // 位置
	XMFLOAT3 normal; // 法線
	XMFLOAT2 uv; // UV座標
};

struct Material {
	XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f }; // アンビエント光の色 0.0f～1.0f
	float pad0 = 0.0f;
	XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; // 拡散反射光の色 0.0f～1.0f
	float pad1 = 0.0f;
	XMFLOAT3 specular = { 1.0f, 1.0, 1.0f }; // 鏡面反射光の色 0.0f～1.0f
	float pad2 = 0.0f;
	float dissolve = 1.0f; // 透明度 0.0～1.0f
	float specularity = 10.0f; // diffuseの強さ 0f～1000f
};

struct SceneData {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
	XMFLOAT3 eye;
};

