#include "engine.hh"
#include "gmenu.hh"
#include "imgui.h"

GameMenu::GameMenu() {
	m_lpMenuCtl = new MenuController;

	auto main = new MenuButtons("InGame Pause menu", true);
	main->AddButton("Resume game", [](MenuBase *) {
		Engine::GetInstance()->SetPause(false);
	});
	main->AddButton("Switch to editor", [](MenuBase *) {
		Engine::GetInstance()->SetRunner(Engine::EDITOR);
	});
	main->AddButton("Work in progress", [](MenuBase *) {});
	main->AddButton("Quit to main menu", [](MenuBase *) {
		Engine::GetInstance()->SetRunner(Engine::MENU);
	});

	m_lpMenuCtl->AddMenu(main);
	m_lpMenuCtl->ShowMenu(main);
}

GameMenu::~GameMenu() {
	delete m_lpMenuCtl;
}

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

	if (m_bMainActive)
		m_lpMenuCtl->Draw();
}
