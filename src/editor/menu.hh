#pragma once

#include "engine.hh"

#include "imgui.h"

class EditorMenu {
public:
	void DrawMainMenu();
	void DrawObjectMenu();
	void Draw();

	inline void Toggle() {
		m_bMainActive = !m_bMainActive;
	}

	Level::ObjectData m_odPicked = {nullptr, nullptr};
private:
	bool m_bMainActive = false;
};
