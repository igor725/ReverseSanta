#pragma once

#include "base_runner.hh"
#include "editor\menu.hh"

class Editor : public BaseRunner {
public:
	Editor();
	~Editor();

	LRESULT OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void OnInput(FLOAT delta, InputState *state);
	void OnUpdate(FLOAT delta);
	void OnDraw(LPDIRECT3DDEVICE9 device);

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

private:
	EditorMenu m_Menu;
};
