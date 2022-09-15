#pragma once

#include "engine.hh"

#include "imgui.h"

class EditorMenu {
public:
	void Draw();

	inline void Toggle() {
		m_bActive = !m_bActive;
	}

	inline bool IsActive() {
		return m_bActive;
	}

private:
	bool m_bActive = false;
};
