#pragma once

#include "menuctl.hh"

class GameMenu {
public:
	GameMenu();
	~GameMenu();

	void Draw();

	inline void SetState(BOOL state) { m_bMainActive = state; }

private:
	MenuController *m_lpMenuCtl = nullptr;
	bool m_bMainActive = false;
};
