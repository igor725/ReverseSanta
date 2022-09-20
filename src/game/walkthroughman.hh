#pragma once

#include <Windows.h>
#include "level.hh"

class WalkthroughMan {
public:
	void NextLevel(Level *level);

private:
	DWORD m_dwCurrLevel = 0;
};
