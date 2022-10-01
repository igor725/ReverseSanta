#pragma once

#include "baserunner.hh"
#include "game\player.hh"
#include "game\gmenu.hh"

class Game : public BaseRunner {
public:
	enum State {
		INVALID,
		PLAYING,
		GAMEOVER_TIME,
		GAMEOVER_LIVES
	};

	Game() : m_lpPlayer(new Player), m_Menu() {}
	~Game() { delete m_lpPlayer; }

	VOID OnOpen(DWORD prev);
	VOID OnClose();
	VOID OnPause(BOOL state);

	VOID OnInput(FLOAT delta, InputState *state);
	VOID OnUpdate(FLOAT delta);
	VOID OnDraw(LPDIRECT3DDEVICE9 device);
	VOID OnDrawUI();

private:
	State m_eState = INVALID;
	Player *m_lpPlayer = nullptr;
	GameMenu m_Menu;
};
