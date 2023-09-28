#include "BasicWindow.h"
#include"SNYELib.h"
#include"Application.h"
#include"Model.h"
#include<Windows.h>
#include<fstream>
#include<iostream>
using namespace std;

/*
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_COMMAND) {
		if (LOWORD(wparam) == BN_CLICKED) {
			MessageBox(hwnd, "ボタンがクリックされました", "メッセージ", MB_OK);
		}
	}
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
*/


/*
LRESULT BasicWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	cout << "BasicWndPro" << endl;
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
*/

// ウィンドウプロシージャ
// 参考:https://suzulang.com/win32api%E3%81%A7%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E3%82%92%E3%82%AB%E3%83%97%E3%82%BB%E3%83%AB%E5%8C%96/
LRESULT CALLBACK BasicWindow::BasicWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	BasicWindow* This = (BasicWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA); //ウィンドウに紐づけられているインスタンスポインタの取得

	if (!This) { //取得ができない -> 紐づけられていない
		if (msg == WM_CREATE) { //ウィンドウ生成時に、クラスインスタンスとウィンドウハンドルを紐づける
			This = (BasicWindow*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)This); //ウィンドウのユーザデータにインスタンスポインタをセットする
				return This->WindowProcedure(hwnd, msg, wparam, lparam);
			}
			else { //lpCreateParamsが0 -> CreateWindowが失敗しており、ウィンドウプロシージャが機能しなくなるのでプログラムを終了させる
				PostQuitMessage(0);
			}
		}
	}
	else { //取得ができた -> 紐づけができている
		cout << "AAA" << endl;
		return This->WindowProcedure(hwnd, msg, wparam, lparam);
		cout << "BBB" << endl;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam); //何にもないときはデフォルトのウィンドウプロシージャへ
}




BasicWindow::BasicWindow(HINSTANCE hi, LPCSTR windowName, unsigned int x, unsigned int y, unsigned int width, unsigned int height) : _hInstance(hi), _windowName(windowName), _x(x), _y(y), _width(width), _height(height) {
	CreateNewWindow();
}

BasicWindow::~BasicWindow() {

}

void BasicWindow::CreateNewWindow() {
	// ウィンドウクラスを生成し、登録
	_windowClass.cbSize = sizeof(WNDCLASSEX);
	_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	_windowClass.lpfnWndProc = BasicWindow::BasicWindowProcedure; // コールバック関数の指定
	_windowClass.cbClsExtra = 0;
	_windowClass.cbWndExtra = 0;
	_windowClass.lpszClassName = _T("SNY ENGINE");// アプリケーションクラス名の指定
	_windowClass.hInstance = _hInstance; // ハンドルを取得し登録
	_windowClass.hIcon = NULL;
	_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_windowClass.hbrBackground = NULL;
	_windowClass.lpszMenuName = NULL;
	_windowClass.hIconSm = NULL;

	RegisterClassEx(&_windowClass); // ウィンドウクラスを登録

	// ウィンドウの大きさを定義
	RECT wrc = { 0, 0, _width, _height }; // ウィンドウの大きさを指定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // ウィンドウを調整

	// ウィンドウオブジェクトの生成
	_hwnd = CreateWindow(
		_windowClass.lpszClassName, // クラス名
		_windowName, // タイトルバー名
		WS_OVERLAPPED,
		//WS_OVERLAPPEDWINDOW, // ウィンドウスタイル
		//CW_USEDEFAULT, // 表示x座標はOSにお任せ
		//CW_USEDEFAULT, // 表示y座標はOSにお任せ
		_x,
		_y,
		wrc.right - wrc.left, // ウィンドウの幅
		wrc.bottom - wrc.top, // ウィンドウの高さ
		NULL, // 親ウィンドウハンドル
		NULL, // メニューハンドル
		_windowClass.hInstance, //呼び出しアプリケーションハンドル
		this // 追加パラメータ
	);
}

void BasicWindow::CreateButton(HWND& hwnd, LPCSTR text, int x, int y, int width, int height, int btnID) {
	hwnd = CreateWindow(
		"BUTTON",
		text, // button text
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // style
		x, // x座標
		y, // y座標
		width, // 幅
		height, // 高さ
		_hwnd, // 親のHWND
		NULL, // MENU
		_hInstance,
		NULL
	);
}