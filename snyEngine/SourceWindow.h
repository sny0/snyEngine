#pragma once
#include "BasicWindow.h"


struct Source {
	string name;
	bool isMTLExist;
	HWND hwnd;
};

class SourceWindow : public BasicWindow {
public:
	SourceWindow(HINSTANCE hi, LPCSTR windowName, unsigned int width, unsigned int x, unsigned int y, unsigned int height);

	~SourceWindow();

	void CreateSourceButtons(map<string, bool> objFiles);

	vector<Source> _sources;

private:
	int _btnWidth = 500;
	int _btnHeight = 30;

public:
	//static LRESULT CALLBACK BasicWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
};
