#pragma once
#include"SNYELib.h"
#include"Model.h"
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



// �����o�ϐ����F_�Z�Z
class Application
{
private:
	WNDCLASSEX _windowClass;
	HWND _hwnd;

	// DXGI
	Microsoft::WRL::ComPtr<IDXGIFactory6> _dxgiFactory = nullptr; // DXGI�̊Ǘ�
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain = nullptr; // �X���b�v�`�F�C��

	// DX12
	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr; // �f�o�C�X(=�g�p����GPU�̏����i�[)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr; // �R�}���h�A���P�[�^(=�R�}���h�����߂Ă����ꏊ)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr; // �R�}���h���X�g(=�R�}���h�����s����Ƃ��ɎQ�Ƃ���C���^�[�t�F�[�X)
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr; // �R�}���h�L���[(=�R�}���h�̊Ǘ�)


	// DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr;

	// �o�b�N�o�b�t�@
	vector<ID3D12Resource*> _backBuffers;

	// �r���[�|�[�g
	CD3DX12_VIEWPORT _viewPort;

	// �V�U�[��`
	CD3DX12_RECT _scissorRect;

	// �t�F���X(=CPU��GPU�̓����ɗ��p)
	Microsoft::WRL::ComPtr<ID3D12Fence> _fence = nullptr;
	// �t�F���X�l
	UINT64 _fenceVal = 0;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr;


	XMMATRIX _worldMat;
	XMMATRIX _viewMat;
	XMMATRIX _projMat;

	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr;

	vector<Model*> _models;

	SceneData* _sceneMap = nullptr;

	// �E�B���h�E�̐���
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);

	// DXGI�֘A(Factory, D3D12Device)�̏�����
	HRESULT InitializeDXGIDevice();

	// �R�}���h�֘A(commandAllocator, commandList, commandQueue)�̏�����
	HRESULT InitializeCommand();

	// �X���b�v�`�F�C���̐���
	HRESULT CreateSwapChain();

	// ��ʂɕ\������RT�𐶐�
	HRESULT CreateFinalRenderTarget();

	HRESULT CreateDepthStencilView();

	HRESULT LoadOBJFile(const char* path, Model& model);

	HRESULT LoadTextureFile(const char* path, Model& model);

	HRESULT CreateVertexBuffer(Model& model);

	HRESULT CreateIndexBuffer(Model& model);

	HRESULT CreateSceneTransformView();

	HRESULT CreateTextureView();

	HRESULT CreateRootSignature();

	HRESULT CreateBasicGraphicsPipeline();

	void SeparateString(string str, vector<string>& vec, const string cs);

public:
	static Application& Instance();

	bool Init();
	void Run();
	void Terminate();
	~Application();
};