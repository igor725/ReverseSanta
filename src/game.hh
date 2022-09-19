#pragma once

#include "baserunner.hh"
#include "game\player.hh"

class Game : public BaseRunner{
public:
	Game();
	~Game();

	void OnOpen();
	void OnClose();

	void OnInput(FLOAT delta, InputState *state);
	void OnUpdate(FLOAT delta);
	void OnDraw(LPDIRECT3DDEVICE9 device);

private:
	Player *m_lpPlayer = nullptr;
};
