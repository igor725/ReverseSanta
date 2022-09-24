#include "gmenu.hh"
#include "imgui.h"

#define DWND_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings

void GameMenu::Draw() {
	if (!m_bMainActive) return;
	const auto vp = ImGui::GetMainViewport();
	const auto res = ImVec2(410, 260);

	ImGui::SetNextWindowPos(ImVec2((vp->WorkSize.x - res.x) * 0.5f, (vp->WorkSize.y - res.y) * 0.5f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(res, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("ReverseSanta Game Menu", &m_bMainActive, DWND_FLAGS)) {
		ImGui::End();
		return;
	}
	/* Телепорт к объектам мира, отображение их координат (наверное), настройки камеры и прочие приколюхи */
	ImGui::End();
}
