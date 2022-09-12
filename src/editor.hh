#pragma once

#include "base_runner.hh"

class Editor : public BaseRunner {
public:
	Editor();
	~Editor();

	bool OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void OnInput(FLOAT delta, InputState *state);
	void OnUpdate(FLOAT delta);
	void OnDraw(LPDIRECT3DDEVICE9 device);

private:
};
