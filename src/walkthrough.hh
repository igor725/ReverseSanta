#pragma once

#include <Windows.h>
#include "level.hh"

class Walkthrough {
public:
	struct Config {
		FLOAT f_fLevelTime = 420.0f;
		BOOL f_bSavePointsEnabled = true, f_bAISnowballs = false,
		f_bTripleJump = false, f_bAllbonuses = false, f_bAirJumpDmg = false;
		DWORD f_dwStartLives = 3;
	};

	Walkthrough(Level *level) : m_lpLevel(level) {}

	VOID Reset();
	VOID Begin();
	DWORD NextLevel();
	BOOL Update(FLOAT delta);

	inline BOOL Death() { return --m_dwLives > 0; }
	inline BOOL SavePointUsed() {
		if (!cfg.f_bSavePointsEnabled) return false;
		m_bSavePointUsed = true;
		return true;
	}

	inline DWORD GetOverallScore() { return m_dwOverallScore; }
	inline Config &GetConfig() { return cfg; }

private:
	Level *m_lpLevel;
	Config cfg;

	DWORD m_dwCurrLevel = 0,
	m_dwOverallScore = 0,
	m_dwLives = 0;
	FLOAT m_fElapsedTime = 0.0f;
	BOOL m_bSavePointUsed = false;
};
