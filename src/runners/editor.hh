#pragma once

#include "baserunner.hh"
#include "editor\emenu.hh"

class Editor : public BaseRunner {
public:
	LRESULT OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	VOID OnInput(FLOAT delta, InputState *state);
	VOID OnDrawUI();

private:
	EditorMenu m_Menu;
};
