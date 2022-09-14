#pragma once

#include "base_runner.hh"

class Game : public BaseRunner{
public:
	Game();
	~Game();

	void OnUpdate(FLOAT delta);
};
