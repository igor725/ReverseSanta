#include "editor\menu.hh"

void EditorMenu::Draw() {
	if (!m_bActive) return;
	const auto vp = ImGui::GetMainViewport();
	const auto res = ImVec2(400, 340);

	ImGui::SetNextWindowPos(ImVec2((vp->WorkSize.x - res.x) * 0.5f, (vp->WorkSize.y - res.y) * 0.5f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(res, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("ReverseSanta Main Menu", &m_bActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Level manager")) {
		static int curr_level = 0;
		// TODO: Итерация по XPK файлу
		static LPSTR items[] = {
			"000.dat", "001.dat", "002.dat", "003.dat", "004.dat", "005.dat",
			"006.dat", "007.dat", "008.dat", "009.dat", "010.dat", "100.dat",
			"demo.dat"
		};

		if (ImGui::ListBox("Pick a level", &curr_level, items, IM_ARRAYSIZE(items), 5)) {
			auto engine = Engine::GetInstance();
			auto level = engine->SysLevel();
			level->Load("levels\\" + std::string(items[curr_level]));
		}
	}

	ImGui::End();
}
