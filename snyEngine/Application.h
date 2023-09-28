#pragma once
#include"SNYELib.h"
#include"BasicWindow.h"
#include"SourceWindow.h"
#include"MainWindow.h"
#include"ModelManager.h"
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
	HINSTANCE _hInstance;
	HINSTANCE GetHINSTANCE();

	unsigned int _mainWindow_x = 300;
	unsigned int _mainWindow_y = 0;
	unsigned int _mainWindow_width = 1280;
	unsigned int _mainWindow_height = 720;

	unsigned int _sourceWindow_x = 0;
	unsigned int _sourceWindow_y = 720;
	unsigned int _sourceWindow_widht = 1580;
	unsigned int _sourceWindow_height = 300;

	unsigned int _modelWindow_x = 0;
	unsigned int _modelWindow_y = 0;
	unsigned int _modelWindow_width = 300;
	unsigned int _modelWindow_height = 720;

	unsigned int _detailWindow_x = 1580;
	unsigned int _detailWindow_y = 0;
	unsigned int _detailWindow_width = 300;
	unsigned int _detailWindow_height = 1220;

	MainWindow* _mainWindow = nullptr;
	//SourceWindow* _sourceWindow = nullptr;
	//BasicWindow* _modelWindow = nullptr;
	//BasicWindow* _detailWindow = nullptr;

private:
	ModelManager _modelManager;
	map<string, bool> _objFiles; // OBJFileName, MTLFile exist
	//vector<Source> _objFiles;


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
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr; // レンダーテクスチャビューのディスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr; // 深度ステンシルビューのディスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr; // 座標変換のビューのディスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _mtlHeap = nullptr; // マテリアルのディスクリプターヒープ

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

	// パイプラインステイト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	// 座標変換行列
	XMMATRIX _worldMat; // ワールド行列
	XMMATRIX _viewMat; // ビュー行列
	XMMATRIX _projMat; // プロジェクション行列

	// バッファー
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr;
	//Microsoft::WRL::ComPtr<ID3D12Resource> _materialBuffer = nullptr;

	// 3Dモデル
	vector<Model*> _models;

	SceneData* _sceneMap = nullptr;

	string _shaderPath = "";
	string _modelPath = "";
	string _shaderPathD = "";
	string _modelPathD = "";


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

	// 深度ステンシルビューの生成
	HRESULT CreateDepthStencilView();

	// OBJファイルのロード
	HRESULT LoadOBJFile(const char* path, Model& model);

	// MTLファイルのロード
	HRESULT LoadMTLFile(const char* path, Model& model);

	// テクスチャファイルのロード
	HRESULT LoadTextureFile(const char* path, Model& model);

	// 頂点バッファーの生成
	HRESULT CreateVertexBuffer(Model& model);

	// インデックスバッファの生成
	HRESULT CreateIndexBuffer(Model& model);

	// マテリアルの（定数）バッファの生成
	HRESULT CreateMaterialBuffer(Model& model);

	// 座標変換のビューの生成
	HRESULT CreateSceneTransformView();

	// テクスチャのビューの生成
	HRESULT CreateTextureView();

	// マテリアルのビューの生成
	HRESULT CreateMaterialView(Model& model);

	// ルートシグネチャの生成
	HRESULT CreateRootSignature();

	// グラフィクスパイプラインの生成
	HRESULT CreateBasicGraphicsPipeline();

	// 文字列の分裂 string csに入っている文字列で分裂させる
	void SeparateString(string str, vector<string>& vec, const string cs);

	void CreatePath();

	void searchOBJFile();

	void searchMTLFile();

public:
	static Application& Instance(HINSTANCE hi);

	// 初期化
	bool Init();

	// Run
	void Run();

	// 終了
	void Terminate();
	~Application();
};