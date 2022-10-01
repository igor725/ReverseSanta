#include "walkthrough.hh"
#include "exceptions.hh"
#include "todo.hh"

static void runlevel(Level *level, DWORD num) {
	char path[16];
	EASSERT(std::snprintf(path, 16, "levels\\%03d.dat", num) > 0 && level->Load(path));
}

VOID Walkthrough::SetDifficulty(Difficulty d) {
	EASSERT(d < DIFF_MAX);
	static const Config m_lpPresets[DIFF_MAX] = {
		{4, 600.0f, true, false, true, false, false},
		{}, // Стандартные настройки сложности
		{2, 320.0f, false, true, false, true, false},
		{1, 200.0f, false, true, false, true, true},
	};

	m_Config = m_lpPresets[d];
	m_eDifficulty = d;
	TODO("Recalc multipliers");
}

VOID Walkthrough::Reset() {
	m_dwLives = m_Config.f_dwStartLives;
	runlevel(m_lpLevel, m_dwCurrLevel = 0);
	Begin();
}

VOID Walkthrough::Begin() {
	m_fElapsedTime = 0;
	m_dwPresentsCount = 0;
	m_bSavePointUsed = false;
	m_dwCollectedPresents = 0;
	m_lpLevel->IterObjects([](Level::ObjectData data, LPVOID ud)->BOOL {
		if (data.f_lpDObj->f_lpElem->f_eType == Elems::BONUS)
			(*(DWORD *)ud)++;

		return false;
	}, &m_dwPresentsCount);
}

BOOL Walkthrough::NextLevel() {
	if (m_Config.f_bAllbonuses && GetPresentsLeft() > 0) return false;

	m_dwOverallScore += CalcScores();
	runlevel(m_lpLevel, ++m_dwCurrLevel);
	Begin();

	return true;
}

DWORD Walkthrough::CalcScores() {
	DWORD score = 0;

	m_lpLevel->IterObjects([](Level::ObjectData data, LPVOID ud)->BOOL {
		switch (data.f_lpDObj->f_lpElem->f_eType) {
			case Elems::BONUS:
				if (data.f_lpDObj->f_bHidden)
					*((DWORD *)ud) += 10;
				break;
		}

		return false;
	}, &score);

	return (score += DWORD(m_Config.f_fLevelTime - m_fElapsedTime) * 2);
}

BOOL Walkthrough::Update(FLOAT delta) {
	m_fElapsedTime += delta;
	return m_fElapsedTime < m_Config.f_fLevelTime;
}
