#include "walkthrough.hh"
#include "exceptions.hh"

static void runlevel(Level *level, DWORD num) {
	char path[16];
	EASSERT(std::snprintf(path, 16, "levels\\%03d.dat", num) > 0 && level->Load(path));
}

VOID Walkthrough::Reset() {
	runlevel(m_lpLevel, m_dwCurrLevel = 0);
	Begin();
}

VOID Walkthrough::Begin() {
	m_fElapsedTime = 0;
	m_dwPresentsCount = 0;
	m_bSavePointUsed = false;
	m_dwCollectedPresents = 0;
	m_dwLives = m_Config.f_dwStartLives;
	m_lpLevel->IterObjects([](Level::ObjectData data, LPVOID ud)->BOOL {
		if (data.f_lpDObj->f_lpElem->f_eType == Elems::BONUS)
			(*(DWORD *)ud)++;

		return false;
	}, &m_dwPresentsCount);
}

DWORD Walkthrough::NextLevel() {
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

	score += DWORD(m_Config.f_fLevelTime - m_fElapsedTime) * 2;
	m_dwOverallScore += score;
	runlevel(m_lpLevel, ++m_dwCurrLevel);
	Begin();

	return score;
}

BOOL Walkthrough::Update(FLOAT delta) {
	m_fElapsedTime += delta;
	return m_fElapsedTime < m_Config.f_fLevelTime;
}
