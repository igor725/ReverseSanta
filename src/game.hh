#pragma once

#include "baserunner.hh"
#include "game\player.hh"
#include "game\gmenu.hh"
#include "game\walkthrough.hh"

class Game : public BaseRunner {
public:
	Game() : m_lpPlayer(new Player) {}
	~Game() { delete m_lpPlayer; }

	VOID OnOpen();
	VOID OnClose();

	inline Player *GetPlayer() { return m_lpPlayer; }

	LRESULT OnWndProc(HWND, UINT iMsg, WPARAM wParam, LPARAM lParam);
	VOID OnInput(FLOAT delta, InputState *state);
	VOID OnUpdate(FLOAT delta);
	VOID OnDraw(LPDIRECT3DDEVICE9 device);
	VOID OnDrawUI();

private:
	BOOL m_bIsPaused = false;
	Player *m_lpPlayer = nullptr;
	Walkthrough m_Walkthrough;
	GameMenu m_Menu;
};
