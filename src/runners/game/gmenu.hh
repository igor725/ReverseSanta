#pragma once

#include "menuctl.hh"

class GameMenu {
public:
	GameMenu();
	~GameMenu();

	void Draw();
	BOOL DrawGOL();
	BOOL DrawGOT();

	inline void SetState(bool state) { m_bMainActive = state; }

private:
	MenuController *m_lpMenuCtl = nullptr;
	bool m_bMainActive = false;
};
