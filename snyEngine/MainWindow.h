#pragma once
#include "BasicWindow.h"

class MainWindow : public BasicWindow {
public:
	MainWindow(HINSTANCE hi, LPCSTR windowName, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	~MainWindow();

public:
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
};