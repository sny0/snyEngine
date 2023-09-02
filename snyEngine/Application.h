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



// メンバ変数名：_〇〇
class Application
{
private:
	WNDCLASSEX _windowClass;
	HWND _hwnd;

	// DXGI
	Microsoft::WRL::ComPtr<IDXGIFactory6> _dxgiFactory = nullptr; // DXGIの管理
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain = nullptr; // スワップチェイン

	// DX12
	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr; // デバイス(=使用するGPUの情報を格納)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr; // コマンドアロケータ(=コマンドをためておく場所)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr; // コマンドリスト(=コマンドを実行するときに参照するインターフェース)
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr; // コマンドキュー(=コマンドの管理)


	// DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr;

	// バックバッファ
	vector<ID3D12Resource*> _backBuffers;

	// ビューポート
	CD3DX12_VIEWPORT _viewPort;

	// シザー矩形
	CD3DX12_RECT _scissorRect;

	// フェンス(=CPUとGPUの同期に利用)
	Microsoft::WRL::ComPtr<ID3D12Fence> _fence = nullptr;
	// フェンス値
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

	// ウィンドウの生成
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);

	// DXGI関連(Factory, D3D12Device)の初期化
	HRESULT InitializeDXGIDevice();

	// コマンド関連(commandAllocator, commandList, commandQueue)の初期化
	HRESULT InitializeCommand();

	// スワップチェインの生成
	HRESULT CreateSwapChain();

	// 画面に表示するRTを生成
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