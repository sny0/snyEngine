#include "MainWindow.h"

MainWindow::MainWindow(HINSTANCE hi, LPCSTR windowName, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	: BasicWindow(hi, windowName, x, y, width, height) {

}

MainWindow::~MainWindow() {

}

LRESULT MainWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	cout << "MAIN WINPRO" << endl;
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hwnd, msg, wparam, lparam);
}