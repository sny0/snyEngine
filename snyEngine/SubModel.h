#pragma once
#include"SNYELib.h"
#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<wrl.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;

class SubModel {
private:
	string _materialName = "";
	Transform _transform;

	vector<Vertex> _vertices;
	Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	vector<unsigned int> _vertexIndices;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};

	TexMetadata _metadata = {};
	ScratchImage _scratchImage = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> _textureBuffer = nullptr;

	string _materialID;

public:
	bool CreateVertexBuffer();

	bool CreateIndexBuffer();

	void Draw();

	SubModel();

	~SubModel();
};