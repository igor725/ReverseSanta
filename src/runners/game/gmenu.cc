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

static BOOL GameOverWin(const char *text) {
	auto &io = ImGui::GetIO();
	auto engine = Engine::GetInstance();
	auto walk = engine->SysWalkthrough();
	DWORD score = walk->GetOverallScore();
	const ImVec2 wcenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	static char buf[48] = {'\0'};

	ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Game over!", nullptr, ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("%s, game over! Your score: %d", text, score);

	BOOL restart = false;
	if (score > 0) {
		ImGui::InputText("Enter your name", buf, IM_ARRAYSIZE(buf));
		if (ImGui::Button("Save to leaderboard") && *buf != '\0')
			if (engine->SysScore()->Put(score, walk->GetDifficulty(), buf)) {
				engine->SysScore()->Save();
				::strcpy_s(buf, "Saved!");
			}
		ImGui::SameLine();
	}
	if (ImGui::Button("Continue playing")) {
		walk->Reset();
		restart = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Exit to main menu"))
		engine->SetRunner(Engine::MENU);
	ImGui::End();

	return restart;
}

BOOL GameMenu::DrawGOL() {
	return GameOverWin("No more lives left");
}

BOOL GameMenu::DrawGOT() {
	return GameOverWin("Time's up");
}

void GameMenu::Draw() {
	auto wt = Engine::GetInstance()->SysWalkthrough();

	ImGui::Begin(
		"HUD", nullptr,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize
	);

	ImGui::Text("Level: %d;", wt->GetLevel() + 1);
	ImGui::SameLine();
	ImGui::Text("Lives: %d;", wt->GetLives());
	ImGui::SameLine();
	ImGui::Text("Presents: %d;", wt->GetPresentsLeft());
	ImGui::SameLine();
	ImGui::Text("Time: %.2f", wt->GetTime());

	ImGui::End();

	if (m_bMainActive)
		m_lpMenuCtl->Draw();
}
