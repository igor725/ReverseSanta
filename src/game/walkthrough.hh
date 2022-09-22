#pragma once

#include <Windows.h>
#include "level.hh"

class Walkthrough {
public:
	VOID Begin();
	DWORD NextLevel(Level *level);
	BOOL Update(FLOAT delta);

	inline BOOL Death() { return --m_dwLives > 0; }
	inline VOID SavePointUsed() { m_bSavePointUsed = true; }

	inline BOOL IsSavePointsEnabled() { return m_bSavePointsEnabled; }
	inline DWORD GetOverallScore() { return m_dwOverallScore; }

private:
	DWORD m_dwCurrLevel = 0,
	m_dwOverallScore = 0,
	m_dwLives = 3;
	const FLOAT m_fLevelTime = 420.0f;
	FLOAT m_fElapsedTime = 0.0f;
	BOOL m_bSavePointUsed = false,
	m_bSavePointsEnabled = true;
};
