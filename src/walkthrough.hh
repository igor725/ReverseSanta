#pragma once

#include <Windows.h>
#include "level.hh"

class Walkthrough {
public:
	enum Difficulty {
		EASY,
		NORMAL,
		HARD,
		VERYHARD,
		DIFF_MAX,

		CUSTOM		
	};

	struct Config {
		FLOAT f_fLevelTime;
		BOOL f_bSavePointsEnabled, f_bAISnowballs,
		f_bTripleJump, f_bAllbonuses, f_bAirJumpDmg;
		DWORD f_dwStartLives;

		Config(DWORD lives = 3, FLOAT time = 420.0f, BOOL save = true,
		BOOL sballs = false, BOOL triple = false, BOOL allbonus = false, BOOL jdmg = false)
		: f_dwStartLives(lives), f_fLevelTime(time), f_bSavePointsEnabled(save),
		f_bAISnowballs(sballs), f_bTripleJump(triple), f_bAllbonuses(allbonus),
		f_bAirJumpDmg(jdmg) {}
	};

	Walkthrough() : m_Config() {}

	VOID Reset();
	VOID Begin();
	BOOL NextLevel();
	DWORD CalcScores();
	BOOL Update(FLOAT delta);
	VOID SetDifficulty(Difficulty lvl);
	inline VOID SetLevel(Level *level) { m_lpLevel = level; }

	inline BOOL Death() { return --m_dwLives < 1; }
	inline VOID CollectLive() { m_dwLives++; }
	inline VOID CollectBonus() { m_dwCollectedPresents++; }
	inline BOOL SavePointUsed() {
		if (!m_Config.f_bSavePointsEnabled) return false;
		m_bSavePointUsed = true;
		return true;
	}

	inline Difficulty GetDifficulty() { return m_eDifficulty; }
	inline DWORD GetLevel() { return m_dwCurrLevel; }
	inline DWORD GetPresentsLeft() { return m_dwPresentsCount - m_dwCollectedPresents; }
	inline FLOAT GetTime() { return m_Config.f_fLevelTime - m_fElapsedTime; }
	inline DWORD GetLives() { return m_dwLives; }
	inline DWORD GetOverallScore() { return m_dwOverallScore; }
	inline Config &GetConfig() { return m_Config; }

private:
	Level *m_lpLevel;
	Config m_Config;

	Difficulty m_eDifficulty = NORMAL;
	DWORD m_dwCurrLevel = 0,
	m_dwOverallScore = 0,
	m_dwLives = 0,
	m_dwCollectedPresents = 0,
	m_dwPresentsCount = 0;
	FLOAT m_fElapsedTime = 0.0f;
	BOOL m_bSavePointUsed = false;
};
