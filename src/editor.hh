#pragma once

#include "baserunner.hh"
#include "editor\menu.hh"

class Editor : public BaseRunner {
public:
	LRESULT OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void OnInput(FLOAT delta, InputState *state);
	void OnDrawUI();

private:
	EditorMenu m_Menu;
};
