#pragma once

#include "baserunner.hh"
#include "game\player.hh"
#include "game\gmenu.hh"

class Game : public BaseRunner {
public:
	Game() : m_lpPlayer(new Player) {}
	~Game() { delete m_lpPlayer; }

	VOID OnOpen(DWORD prev);
	VOID OnClose();

	inline Player *GetPlayer() { return m_lpPlayer; }

	LRESULT OnWndProc(HWND, UINT iMsg, WPARAM wParam, LPARAM lParam);
	VOID OnInput(FLOAT delta, InputState *state);
	VOID OnUpdate(FLOAT delta);
	VOID OnDraw(LPDIRECT3DDEVICE9 device);
	VOID OnDrawUI();

private:
	Player *m_lpPlayer = nullptr;
	GameMenu m_Menu;
};
