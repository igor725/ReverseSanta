#pragma once

class GameMenu {
public:
	void Draw();

	inline void Toggle() {
		m_bMainActive = !m_bMainActive;
	}

private:
	bool m_bMainActive = false;
};
