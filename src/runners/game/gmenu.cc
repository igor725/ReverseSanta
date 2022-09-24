#include "engine.hh"
#include "gmenu.hh"
#include "imgui.h"

void GameMenu::Draw() {
	auto wt = Engine::GetInstance()->SysWalkthrough();

	ImGui::Begin(
		"HUD", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar
	);

	ImGui::Text("Lives: %d", wt->GetLives());
	ImGui::SameLine();
	ImGui::Text("Presents left: %d", wt->GetPresentsLeft());
	ImGui::SameLine();
	ImGui::Text("Time left: %.2f", wt->GetTime());

	ImGui::End();

	if (!m_bMainActive) return;
	const auto vp = ImGui::GetMainViewport();
	const auto res = ImVec2(410, 260);

	ImGui::SetNextWindowPos(ImVec2((vp->WorkSize.x - res.x) * 0.5f, (vp->WorkSize.y - res.y) * 0.5f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(res, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("ReverseSanta Game Menu", &m_bMainActive, ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}
	/* Телепорт к объектам мира, отображение их координат (наверное), настройки камеры и прочие приколюхи */
	ImGui::End();
}
