#pragma once
#include<string>
#include<Windows.h>
#include<DirectXMath.h>

using namespace DirectX;


struct Vertex {
	XMFLOAT3 position; // Êu
	XMFLOAT3 normal; // @ü
	XMFLOAT2 uv; // UVÀW
};

struct Material {
	XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f }; // ArGgõÌF 0.0f`1.0f
	float pad0 = 0.0f;
	XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; // gU½ËõÌF 0.0f`1.0f
	float pad1 = 0.0f;
	XMFLOAT3 specular = { 1.0f, 1.0, 1.0f }; // ¾Ê½ËõÌF 0.0f`1.0f
	float pad2 = 0.0f;
	float dissolve = 1.0f; // §¾x 0.0`1.0f
	float specularity = 10.0f; // diffuseÌ­³ 0f`1000f
};

struct SceneData {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
	XMFLOAT3 eye;
};

struct Transform {
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
};
/*
class Source {
public:
	HWND hwnd;
	string name;
	bool isMTLExist = false;
};
*/