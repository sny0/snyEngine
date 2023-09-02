
#include"SNYELib.h"
#include"Application.h"
#include"Model.h"
#include<Windows.h>
#include<fstream>
#include<iostream>
using namespace std;


const unsigned int window_width = 1280;
const unsigned int window_height = 720;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// �f�o�b�O���C���[��L���ɂ���
void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}

void Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass) {
	// �E�B���h�E�N���X�𐶐����A�o�^
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure; // �R�[���o�b�N�֐��̎w��
	windowClass.lpszClassName = _T("SNY ENGINE");// �A�v���P�[�V�����N���X���̎w��
	windowClass.hInstance = GetModuleHandle(0); // �n���h�����擾���o�^
	RegisterClassEx(&windowClass); // �E�B���h�E�N���X��o�^

	// �E�B���h�E�̑傫�����`
	RECT wrc = { 0, 0, window_width, window_height }; // �E�B���h�E�̑傫�����w��
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �E�B���h�E�𒲐�

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(windowClass.lpszClassName, // �N���X��
		_T("SNY ENGINE mainWindow"), // �^�C�g���o�[��
		WS_OVERLAPPEDWINDOW, // �E�B���h�E�X�^�C��
		CW_USEDEFAULT, // �\��x���W��OS�ɂ��C��
		CW_USEDEFAULT, // �\��y���W��OS�ɂ��C��
		wrc.right - wrc.left, // �E�B���h�E�̕�
		wrc.bottom - wrc.top, // �E�B���h�E�̍���
		nullptr, // �e�E�B���h�E�n���h��
		nullptr, // ���j���[�n���h��
		windowClass.hInstance, //�Ăяo���A�v���P�[�V�����n���h��
		nullptr); // �ǉ��p�����[�^
}

HRESULT Application::InitializeDXGIDevice() {
	// DXGIFactory�̏�����
	UINT flagsDXGI = 0;
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG; // DXGI�̃G���[��\���ł���悤�ɂ���
	HRESULT result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
	// ���s������I��
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �g�p����A�_�v�^�[(=GPU)�����߂�
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	// PC�ɓ��ڂ���Ă���A�_�v�^�[(=GPU)���擾���A�L�^����
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		adapters.push_back(tmpAdapter);
	}

	// NVIDIA�̃A�_�v�^�[��������
	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	result = S_FALSE;
	D3D_FEATURE_LEVEL featureLevel;

	// �t�B�[�`���[���x���̗�
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// �A�_�v�^�[�ɓK������t�B�[�`���[���x����T���A�f�o�C�X��������
	for (auto l : levels) {
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())))) {
			featureLevel = l;
			result = S_OK;
			break;
		}
	}
	return result;
}

HRESULT Application::InitializeCommand() {
	// �R�}���h�A���P�[�^�[�̐���
	HRESULT result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �R�}���h���X�g�̐���
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �R�}���h�L���[�̐���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}
	return 0;
}

HRESULT Application::CreateSwapChain() {
	// �X���b�v�`�F�C���̐���
	DXGI_SWAP_CHAIN_DESC1  swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue.Get(),
		_hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

HRESULT Application::CreateFinalRenderTarget() {
	// RTV�p�̃f�B�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; // �_�u���o�b�t�@
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �X���b�v�`�F�[���̏����擾
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	_backBuffers.resize(swcDesc.BufferCount); // �X���b�v�`�F�[���̃o�b�t�@�[���ɍ��킹�ăo�b�N�o�b�t�@�̐���ύX

	// �f�B�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// RTV��ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	for (size_t i = 0; i < swcDesc.BufferCount; i++) {
		result = _swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&_backBuffers[i])); // �X���b�v�`�F�[���̃o�b�N�o�b�t�@�̃A�h���X���L�^
		assert(SUCCEEDED(result));
		rtvDesc.Format = _backBuffers[i]->GetDesc().Format;
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle); // RTV�𐶐�
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // RTV�̑傫�����n���h�������炷
	}

	// �r���[�|�[�g��ݒ�
	_viewPort = CD3DX12_VIEWPORT(_backBuffers[0]);
	
	// �V�U�[��`��ݒ�
	_scissorRect = CD3DX12_RECT(0, 0, window_width, window_height);
	
	return result;
}

bool Application::Init() {
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
	CreateGameWindow(_hwnd, _windowClass);
#ifdef _DEBUG
	EnableDebugLayer();
#endif

	// DXGI�֘A�̏�����
	if (FAILED(InitializeDXGIDevice())) {
		assert(0);
		return false;
	}

	// �R�}���h�֘A�̏�����
	if (FAILED(InitializeCommand())) {
		assert(0);
		return false;
	}

	// �X���b�v�`�F�[���𐶐�
	if (FAILED(CreateSwapChain())) {
		assert(0);
		return false;
	}

	// �ŏI�I�ɕ\������RTV��ݒ�
	if (FAILED(CreateFinalRenderTarget())) {
		assert(0);
		return false;
	}

	// �t�F���X�̏�����
	if (FAILED(_dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	if (FAILED(CreateDepthStencilView())) {
		assert(0);
		return false;
	}

	
	return true;
}

void Application::Run() {
	ShowWindow(_hwnd, SW_SHOW);

	/*
	Model model;

	LoadOBJFile("", model);

	CreateVertexBuffer(model);
	CreateIndexBuffer(model);
	*/
	Model model;

	_models.push_back(&model);

	if (FAILED(LoadOBJFile("", model))) {
		assert(0);
		//return false;
	}
	if (FAILED(CreateVertexBuffer(model))) {
		assert(0);
		//return false;
	}

	if (FAILED(CreateIndexBuffer(model))) {
		assert(0);
		//return false;
	}

	if (FAILED(LoadTextureFile("", model))) {
		assert(0);
	}

	if (FAILED(CreateRootSignature())) {
		assert(0);
		//return false;
	}

	if (FAILED(CreateBasicGraphicsPipeline())) {
		assert(0);
		//return false;
	}

	if (FAILED(CreateSceneTransformView())) {
		assert(0);
		//return false;
	}

	if (FAILED(CreateTextureView())) {
		assert(0);
	}

	//cout << "print v" << endl;
	for (int i = 0; i<_models[0]->_vertices.size(); i++) {
		cout << i << " " << _models[0]->_vertices[i].position.x << " " << _models[0]->_vertices[i].position.y << " " << _models[0]->_vertices[i].position.z << endl;
	}

	//cout << "print i" << endl;
	for (int i = 0; i < _models[0]->_vertexIndices.size()/3; i++) {
		cout << i << " " << _models[0]->_vertexIndices[i * 3] << " " << _models[0]->_vertexIndices[i * 3 + 1] << " " << _models[0]->_vertexIndices[i * 3 + 2] << endl;
	}
	//cout << "c" << endl;
	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	float angle = 0.0f;
	MSG msg = {}; // �E�B���h�E���b�Z�[�W

	while (true) {
		_worldMat = XMMatrixRotationY(angle);
		_sceneMap->world = _worldMat;
		_sceneMap->view = _viewMat;
		_sceneMap->proj = _projMat;
		angle += 0.01f;


		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_LBUTTONDOWN) {
#ifdef _DEBUG
			printf("press left button");
#endif
		}

		// �A�v���P�[�V�����̏I�����b�Z�[�W�����͂��ꂽ��I��
		if (msg.message == WM_QUIT) {
			break;
		}


		/*
		if (GetAsyncKeyState('A') && 0x800) {

		}
		*/
		
		//cout << "e" << endl;

		// �y�R�}���h���v�b�V���z�����_�[�^�[�Q�b�g���w��
		UINT bbIdx = _swapchain->GetCurrentBackBufferIndex(); // ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾

		// �o�b�N�o�b�t�@�[�Ƀo���A��ݒ�
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmdList->ResourceBarrier(1, &barrier);


		_cmdList->SetPipelineState(_pipelineState.Get());

		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart(); // RTV�̃f�B�X�N���v�^�[�q�[�v�̃n���h�����擾
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // ���݂̃o�b�N�o�b�t�@��RTV�n���h�����擾
		_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//�y�R�}���h���v�b�V���z��ʂ��N���A
		float clearColor[] = { 0.0f, 0.5f, 0.0f, 1.0f }; // RBGA
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		
		_cmdList->RSSetViewports(1, &_viewPort);

		_cmdList->RSSetScissorRects(1, &_scissorRect);

		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());


		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		_cmdList->IASetVertexBuffers(0, 1, &_models[0]->_vertexBufferView);
		//_cmdList->IASetVertexBuffers(0, 1, &vbView);

		_cmdList->IASetIndexBuffer(&_models[0]->_indexBufferView);
		//_cmdList->IASetIndexBuffer(&ibView);

		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());



		ID3D12DescriptorHeap* bdh[] = { _basicHeap.Get() };
		_cmdList->SetDescriptorHeaps(1, bdh);
		auto basicHeapH = _basicHeap->GetGPUDescriptorHandleForHeapStart();
		_cmdList->SetGraphicsRootDescriptorTable(0, basicHeapH);


		//basicHeapH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//_cmdList->SetGraphicsRootDescriptorTable(1, basicHeapH);

		//cout << "f" << endl;
		_cmdList->DrawIndexedInstanced(_models[0]->_vertexIndices.size(), 1, 0, 0, 0);
		//_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
		//cout << "g" << endl;

		//cout << _models[0]->_vertexIndices.size() << endl;

		//cout << "d" << endl;

		// �o�b�N�o�b�t�@�[�̃o���A��Present��Ԃɖ߂�
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		_cmdList->ResourceBarrier(1, &barrier);

		// �R�}���h���͂̏I��
		_cmdList->Close();

		// �R�}���h�̎��s
		ID3D12CommandList* cmdLists[] = { _cmdList.Get() };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// �t�F���X���g�p���AGPU�̏�����҂�
		_cmdQueue->Signal(_fence.Get(), ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal) { // CPU�̃t�F���X�l��GPU�̃t�F���X�l���قȂ鎞
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// �R�}���h���X�g���N���A
		_cmdAllocator->Reset();
		_cmdList->Reset(_cmdAllocator.Get(), nullptr);

		// �t���b�v(��ʂ̐؂�ւ�)
		_swapchain->Present(1, 0); // 1�t���[���Ő؂�ւ���

	}
}

void Application::Terminate() {
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}

Application& Application::Instance() {
	static Application instance;
	return instance;
}

Application::~Application() {

}

HRESULT Application::LoadOBJFile(const char* path, Model& model){

	cout << "start loadOBJFileMethod" << endl;
	ifstream ifs("D:/GitHub/snyEngine/snyEngine/model/horse.obj");

	
	if (!ifs) {
		//char strerr[256];
		//strerror_s(strerr, 256, ifs);
		//MessageBox(_hwnd, strerr, "Open Error", MB_ICONERROR);
		cout << "Don't Open File.";
		assert(0);
		return E_ABORT;
	}
	
	cout << "Open file." << endl;
	
	string objFileName;
	vector<string> memo;
	vector<XMFLOAT3> v;
	vector<XMFLOAT2> vt;
	vector<XMFLOAT3> vn;
	vector<vector<XMINT3>> f;
	vector<int> fs;

	string line;
	cout << "start read file" << endl;
	int k = 0;
	while (getline(ifs, line)) {
		cout << k++ << endl;

		if (line[0] == '#') {
			memo.push_back(line.substr(1, string::npos));
		}
		else {
			vector<string> strs;
			strs.clear();
			SeparateString(line, strs, " ");

			if (strs[0] == "v") {
				XMFLOAT3 tmpv = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
				v.push_back(tmpv);
			}
			else if (strs[0] == "vt") {
				XMFLOAT2 tmpvt = { stof(strs[1]), stof(strs[2]) };
				vt.push_back(tmpvt);
			}
			else if (strs[0] == "vn") {
				XMFLOAT3 tmpvn = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
				vn.push_back(tmpvn);
			}
			else if (strs[0] == "f") {
				vector<string> strs1;
				vector<XMINT3> tmpf;
				for (int i = 0; i < strs.size() - 1; i++) {
					strs1.clear();
					SeparateString(strs[i + 1], strs1, "/");

					XMINT3 tmpvd = { stoi(strs1[0]), stoi(strs1[1]), stoi(strs1[2]) };
					tmpf.push_back(tmpvd);
				}
				f.push_back(tmpf);
				fs.push_back(strs.size()-1);
			}
			else if (strs[0] == "usemtl") {

			}
			else {

			}
		}
	}

	cout << "finish read file";

	ifs.close();
	cout << "Num v:" << v.size() << " vt:" << vt.size() << " vn:" << vn.size() << " f:" << f.size() << endl;
	for (int i = 0; i < f.size(); i++) {
		cout << "i:" << i << " num:" << f[i].size() << endl;
	}

	std::cout << "close file" << endl;

	cout << "make VertexStrct" << endl;

	int offset = 0;
	for (int i = 0; i < f.size(); i++) {
		for (int j = 0; j < f[i].size(); j++) {
			Vertex tmpVert = {};

			tmpVert.position = v[f[i][j].x - 1];
			tmpVert.uv = vt[f[i][j].y - 1];
			tmpVert.normal = vn[f[i][j].z - 1];
			model._vertices.push_back(tmpVert);
			cout << "i:" << i << "  j:" << j << "  vertex:" << tmpVert.position.x <<" " << tmpVert.position.y << " " << tmpVert.position.z << endl;
		}

		for (int j = 0; f[i].size() - j >= 3; j++) {
			model._vertexIndices.push_back(offset);
			model._vertexIndices.push_back(offset+1+j);
			model._vertexIndices.push_back(offset+2+j);
			cout << offset << " " << offset + 1 + j << " " << offset + 2 + j << endl;
		}
		offset += f[i].size();
	}
	cout << "finish making vertexstrct" << endl;

	cout << endl;
	cout << "Print Vertices Data." << endl;
	for (int i = 0; i < model._vertices.size(); i++) {
		cout << i << ": " << model._vertices[i].position.x << " " << model._vertices[i].position.y << " " << model._vertices[i].position.z << endl;
	}
	cout << endl;
	cout << "Print VertexIndices Data." << endl;

	for (int i = 0; i < model._vertexIndices.size() / 3; i++) {
		cout << model._vertexIndices[i*3] << " " << model._vertexIndices[i*3 + 1] << " " << model._vertexIndices[i*3 + 2] << endl;
	}

	/*
	vector<Vertex> tmpv = {
		{{-1.0f,-1.0f,0.0f},{0.0f, 0.0f, 0.0f}, {0.0f,1.0f} },//����
		{{-1.0f,1.0f,0.0f} ,{0.0f, 0.0f, 0.0f},{0.0f,0.0f}},//����
		{{1.0f,-1.0f,0.0f} ,{0.0f, 0.0f, 0.0f},{1.0f,1.0f}},//�E��
		{{1.0f,1.0f,0.0f} ,{0.0f, 0.0f, 0.0f},{1.0f,0.0f}},//�E��
	};

	vector<unsigned int> tmpi = { 0,1,2, 2,1,3 };
	model._vertices = tmpv;
	model._vertexIndices = tmpi;
	*/
	return S_OK;

}

HRESULT Application::LoadTextureFile(const char* path, Model& model) {
	HRESULT result = LoadFromWICFile(L"D:/GitHub/snyEngine/snyEngine/model/snyicon.png",
		WIC_FLAGS_NONE,
		&model._metadata,
		model._scratchImage
	);

	if (FALSE(result)) {
		assert(0);
		return result;
	}

	auto img = model._scratchImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	CD3DX12_RESOURCE_DESC resDesc = {};
	resDesc.Format = model._metadata.format;
	resDesc.Width = static_cast<UINT>(model._metadata.width);
	resDesc.Height = static_cast<UINT>(model._metadata.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(model._metadata.arraySize);
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.MipLevels = static_cast<UINT16>(model._metadata.mipLevels);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(model._metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(model._textureBuffer.ReleaseAndGetAddressOf())
	);

	result = model._textureBuffer->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		static_cast<UINT>(img->rowPitch),
		static_cast<UINT>(img->slicePitch)
	);
	return result;
}

HRESULT Application::CreateDepthStencilView() {
	auto depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		window_width,
		window_height,
		1,
		1,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	CD3DX12_CLEAR_VALUE rtClearValue(DXGI_FORMAT_R8G8B8A8_UINT, clearColor);

	HRESULT result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	_dev->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

HRESULT Application::CreateVertexBuffer(Model& model) {
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(model._vertices.size() * sizeof(Vertex));

	HRESULT result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model._vertexBuffer.ReleaseAndGetAddressOf())
	);

	Vertex* vertMap = nullptr;
	result = model._vertexBuffer->Map(0, nullptr, (void**)&vertMap);
	copy(begin(model._vertices), end(model._vertices), vertMap);
	cout << "vertices num:" << model._vertices.size() << endl;
	cout << "check vertMap" << endl;
	for (int i = 0; i < model._vertices.size(); i++) {
		cout << i+1 <<  " pos: " << vertMap[i].position.x << " " << vertMap[i].position.y << " " << vertMap[i].position.z << endl;
		cout << i+1 <<  " nor: " << vertMap[i].normal.x << " " << vertMap[i].normal.y << " " << vertMap[i].normal.z << endl;
		cout << i+1 <<  " uv : " << vertMap[i].uv.x << " " << vertMap[i].uv.y << endl;
	}
	model._vertexBuffer->Unmap(0, nullptr);

	model._vertexBufferView.BufferLocation = model._vertexBuffer->GetGPUVirtualAddress();
	model._vertexBufferView.SizeInBytes = model._vertexBuffer->GetDesc().Width;
	cout << "size of vbv: " << model._vertexBufferView.SizeInBytes << endl;
	model._vertexBufferView.StrideInBytes = sizeof(Vertex);
	cout << " one size of vbv: " << model._vertexBufferView.StrideInBytes << endl;



	return result;
}

HRESULT Application::CreateIndexBuffer(Model& model) {
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(model._vertexIndices.size() * sizeof(unsigned int));

	HRESULT result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,

		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model._indexBuffer.ReleaseAndGetAddressOf())
	);

	unsigned int* indeMap = nullptr;
	result = model._indexBuffer->Map(0, nullptr, (void**)&indeMap);
	copy(begin(model._vertexIndices), end(model._vertexIndices), indeMap);
	cout << "check indexMap" << endl;
	for (int i = 0; i < model._vertexIndices.size()/3; i++) {
		cout << indeMap[3*i] << " " << indeMap[3*i + 1] << " " << indeMap[3*i + 2] << endl;
	}
	cout << endl;
	model._indexBuffer->Unmap(0, nullptr);

	model._indexBufferView.BufferLocation = model._indexBuffer->GetGPUVirtualAddress();
	model._indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	model._indexBufferView.SizeInBytes = model._indexBuffer->GetDesc().Width;

	return result;
}

HRESULT Application::CreateSceneTransformView() {
	_worldMat = XMMatrixIdentity();
	//_worldMat = XMMatrixRotationY(XM_PIDIV4);
	XMFLOAT3 eye(0, 0, -3);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);

	_viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	_projMat = XMMatrixPerspectiveFovLH(XM_PIDIV2,
		static_cast<float>(window_width) / static_cast<float>(window_height),
		1.0f,
		1000.0f
	);

	auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);
	HRESULT result = _dev->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}


	result = _constBuffer->Map(0, nullptr, (void**)&_sceneMap);

	_sceneMap->world = _worldMat;
	_sceneMap->view = _viewMat;
	_sceneMap->proj = _projMat;
	_sceneMap->eye = eye;


	// �f�B�X�N���v�^�[�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_basicHeap.ReleaseAndGetAddressOf()));

	auto basicHeapHandle = _basicHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_constBuffer->GetDesc().Width);

	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return result;
}

HRESULT Application::CreateTextureView() {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = _models[0]->_metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto basicHeapH = _basicHeap->GetCPUDescriptorHandleForHeapStart();

	basicHeapH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_dev->CreateShaderResourceView(
		_models[0]->_textureBuffer.Get(),
		&srvDesc,
		basicHeapH
	);

	return S_OK;
}

HRESULT Application::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descTblRanges[2] = {};
	descTblRanges[0].NumDescriptors = 1;
	descTblRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRanges[0].BaseShaderRegister = 0;
	descTblRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRanges[1].NumDescriptors = 1;
	descTblRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRanges[1].BaseShaderRegister = 0;
	descTblRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/*
	CD3DX12_DESCRIPTOR_RANGE descTblRanges[2] = {};
	descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	*/

	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRanges[0];
	rootparam.DescriptorTable.NumDescriptorRanges = 2;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//CD3DX12_ROOT_PARAMETER rootParams[1] = {};
	//rootParams[0].InitAsDescriptorTable(2, &descTblRanges[0]);

	/*
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[1] = {};
	samplerDescs[0].Init(0);


	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(1, rootparam, 1, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	*/

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;
	rootSignatureDesc.NumParameters = 1;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}
	return result;

}

HRESULT Application::CreateBasicGraphicsPipeline() {
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(L"BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errorBlob
	);
	
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	result = D3DCompileFromFile(L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob
	);

	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = _rootSignature.Get();
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�Ȃ�

	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	result = _dev->CreateGraphicsPipelineState(&gpipeline,
		IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
	}
	return result;
}

void Application::SeparateString(string str, vector<string>& vec, const string cs) {
	int offset = 0;
	int tmpPos = str.find(cs, offset);

	while (tmpPos != -1) {
		if(tmpPos != offset) vec.push_back(str.substr(offset, tmpPos - offset));
		offset = tmpPos+1;
		tmpPos = str.find(cs, offset);
	}
	if (offset < str.size()) {
		vec.push_back(str.substr(offset, str.size()-offset));
	}
}
