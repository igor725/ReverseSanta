#pragma once

#include "baserunner.hh"
#include "game\player.hh"
#include "game\gmenu.hh"
#include "game\walkthroughman.hh"

class Game : public BaseRunner {
public:
	Game();
	~Game();

	void OnOpen();
	void OnClose();
	
	LRESULT OnWndProc(HWND, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void OnInput(FLOAT delta, InputState *state);
	void OnUpdate(FLOAT delta);
	void OnDraw(LPDIRECT3DDEVICE9 device);
	void OnDrawUI();

private:
	Player *m_lpPlayer = nullptr;
	WalkthroughMan m_Walkthrough;
	GameMenu m_Menu;
};
