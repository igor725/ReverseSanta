#include "game\walkthrough.hh"
#include "exceptions.hh"

VOID Walkthrough::Begin() {
	m_dwLives = 3;
	m_fElapsedTime = 0;
	m_bSavePointUsed = false;
}

DWORD Walkthrough::NextLevel(Level *level) {
	DWORD score = 0;

	level->IterObjects([](Level::ObjectData data, LPVOID ud)->BOOL {
		switch (data.f_lpDObj->f_lpElem->f_eType) {
			case Elems::BONUS:
				if (data.f_lpDObj->f_bHidden)
					*((DWORD *)ud) += 10;
				break;
		}

		return false;
	}, &score);

	score += DWORD(m_fLevelTime - m_fElapsedTime) * 2;
	m_dwOverallScore += score;
	m_fElapsedTime = 0.0f;

	char path[16];
	EASSERT(std::snprintf(path, 16, "levels\\%03d.dat", ++m_dwCurrLevel) > 0 && level->Load(path));

	return score;
}

BOOL Walkthrough::Update(FLOAT delta) {
	m_fElapsedTime += delta;
	return m_fElapsedTime < m_fLevelTime;
}
