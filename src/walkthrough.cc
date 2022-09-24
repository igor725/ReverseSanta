#include "walkthrough.hh"
#include "exceptions.hh"

static void runlevel(Level *level, DWORD num) {
	char path[16];
	EASSERT(std::snprintf(path, 16, "levels\\%03d.dat", num) > 0 && level->Load(path));
}

VOID Walkthrough::Reset() {
	Begin();
	runlevel(m_lpLevel, m_dwCurrLevel = 0);
}

VOID Walkthrough::Begin() {
	m_fElapsedTime = 0;
	m_bSavePointUsed = false;
	m_dwLives = cfg.f_dwStartLives;
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

	score += DWORD(cfg.f_fLevelTime - m_fElapsedTime) * 2;
	m_dwOverallScore += score;
	m_fElapsedTime = 0.0f;
	runlevel(m_lpLevel, ++m_dwCurrLevel);

	return score;
}

BOOL Walkthrough::Update(FLOAT delta) {
	m_fElapsedTime += delta;
	return m_fElapsedTime < cfg.f_fLevelTime;
}
