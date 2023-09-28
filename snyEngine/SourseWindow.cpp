#include "SourceWindow.h"

SourceWindow::SourceWindow(HINSTANCE hi, LPCSTR windowName, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	: BasicWindow(hi, windowName, x, y, width, height) {

}

SourceWindow::~SourceWindow() {

}

void SourceWindow::CreateSourceButtons(map<string, bool> objFiles) {
	_sources.clear();
	std::cout << "a" << endl;
	for (auto f : objFiles) {
		Source tmpSrc;
		tmpSrc.name = f.first;
		tmpSrc.isMTLExist = f.second;
		_sources.push_back(tmpSrc);
		cout << "f" << endl;
	}

	int x = 0;
	int y = 0;
	for (int i = 0; i < _sources.size(); i++) {
		string btnNameStr = _sources[i].name + "(OBJ";
		if (_sources[i].isMTLExist) {
			btnNameStr += ", MTL";
		}
		btnNameStr += ")";
		LPCSTR btnName = (LPCSTR)btnNameStr.c_str();

		CreateButton(_sources[i].hwnd, btnName, x, y, _btnWidth, _btnHeight, i);
		y += _btnHeight;
		std::cout << "aaaaa" << std::endl;
	}


}


LRESULT SourceWindow::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	cout << "sourceWindowPro" << endl;
	switch (msg) {
	case WM_COMMAND:
		int wmId = LOWORD(wparam);
		cout << "wmId:" << wmId << endl;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}