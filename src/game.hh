#pragma once

#include "base_runner.hh"
#include "game\player.hh"

class Game : public BaseRunner{
public:
	Game();
	~Game();

	void OnUpdate(FLOAT delta);
	void OnDraw(LPDIRECT3DDEVICE9 device);

private:
	Player *m_lpPlayer = nullptr;
};
