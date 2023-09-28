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



// �����o�ϐ����F_�Z�Z
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
	Microsoft::WRL::ComPtr<IDXGIFactory6> _dxgiFactory = nullptr; // DXGI�̊Ǘ�
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain = nullptr; // �X���b�v�`�F�C��

	// DX12
	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr; // �f�o�C�X(=�g�p����GPU�̏����i�[)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr; // �R�}���h�A���P�[�^(=�R�}���h�����߂Ă����ꏊ)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr; // �R�}���h���X�g(=�R�}���h�����s����Ƃ��ɎQ�Ƃ���C���^�[�t�F�[�X)
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr; // �R�}���h�L���[(=�R�}���h�̊Ǘ�)


	// DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr; // �����_�[�e�N�X�`���r���[�̃f�B�X�N���v�^�[�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr; // �[�x�X�e���V���r���[�̃f�B�X�N���v�^�[�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr; // ���W�ϊ��̃r���[�̃f�B�X�N���v�^�[�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _mtlHeap = nullptr; // �}�e���A���̃f�B�X�N���v�^�[�q�[�v

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

	// �p�C�v���C���X�e�C�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

	// ���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	// ���W�ϊ��s��
	XMMATRIX _worldMat; // ���[���h�s��
	XMMATRIX _viewMat; // �r���[�s��
	XMMATRIX _projMat; // �v���W�F�N�V�����s��

	// �o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr;
	//Microsoft::WRL::ComPtr<ID3D12Resource> _materialBuffer = nullptr;

	// 3D���f��
	vector<Model*> _models;

	SceneData* _sceneMap = nullptr;

	string _shaderPath = "";
	string _modelPath = "";
	string _shaderPathD = "";
	string _modelPathD = "";


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

	// �[�x�X�e���V���r���[�̐���
	HRESULT CreateDepthStencilView();

	// OBJ�t�@�C���̃��[�h
	HRESULT LoadOBJFile(const char* path, Model& model);

	// MTL�t�@�C���̃��[�h
	HRESULT LoadMTLFile(const char* path, Model& model);

	// �e�N�X�`���t�@�C���̃��[�h
	HRESULT LoadTextureFile(const char* path, Model& model);

	// ���_�o�b�t�@�[�̐���
	HRESULT CreateVertexBuffer(Model& model);

	// �C���f�b�N�X�o�b�t�@�̐���
	HRESULT CreateIndexBuffer(Model& model);

	// �}�e���A���́i�萔�j�o�b�t�@�̐���
	HRESULT CreateMaterialBuffer(Model& model);

	// ���W�ϊ��̃r���[�̐���
	HRESULT CreateSceneTransformView();

	// �e�N�X�`���̃r���[�̐���
	HRESULT CreateTextureView();

	// �}�e���A���̃r���[�̐���
	HRESULT CreateMaterialView(Model& model);

	// ���[�g�V�O�l�`���̐���
	HRESULT CreateRootSignature();

	// �O���t�B�N�X�p�C�v���C���̐���
	HRESULT CreateBasicGraphicsPipeline();

	// ������̕��� string cs�ɓ����Ă��镶����ŕ��􂳂���
	void SeparateString(string str, vector<string>& vec, const string cs);

	void CreatePath();

	void searchOBJFile();

	void searchMTLFile();

public:
	static Application& Instance(HINSTANCE hi);

	// ������
	bool Init();

	// Run
	void Run();

	// �I��
	void Terminate();
	~Application();
};