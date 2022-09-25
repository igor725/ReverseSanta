#include "engine.hh"
#include "exceptions.hh"
#include "menu.hh"
#include "imgui.h"

#define DWND_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse

class MenuOptions : public MenuBase {
public:
	MenuOptions(std::string title) : MenuBase(title) {}

	void Draw() {
		if (!m_bShowSettings) {
			GetController()->CloseMenu();
			m_bShowSettings = true;
			return;
		}

		auto &io = ImGui::GetIO();
		auto engine = Engine::GetInstance();
		const ImVec2 wsize = ImVec2(404, 260); // Придумать, как вынести весь этот прикол отсюда
		const ImVec2 wcenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

		ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Settings", &m_bShowSettings, DWND_FLAGS);
		if (ImGui::CollapsingHeader("Diffulty")) {
			auto &walk = engine->SysWalkthrough()->GetConfig();
			static const char *diffs[] = {"Easy", "Normal", "Hard", "Very hard", "Custom"};
			static const char *discr[] = {
				"Each air jump takes 5 seconds off the timer",
				"Enemies throwing snowballs",
				"Collect all presents to be able to finish the level",
				"Extra lives and savepoints are available",
				"Double air jump allowed",
				"Available lives %d",
				"Time per level: %.2f sec"
			};
			static const Walkthrough::Config presets[] = {
				{4, 600.0f, true, false, true, false, false},
				{}, // Стандартные настройки сложности
				{2, 320.0f, false, true, false, true, false},
				{1, 200.0f, false, true, false, true, true},
			};
			static int current = 1;
			if (ImGui::ListBox("Select difficulty", &current, diffs, IM_ARRAYSIZE(diffs)))
				if (current < 4) walk = presets[current];

			ImGui::Spacing();
			if (current == 4) {
				ImGui::Checkbox(discr[0], (bool *)&walk.f_bAirJumpDmg);
				ImGui::Checkbox(discr[1], (bool *)&walk.f_bAISnowballs);
				ImGui::Checkbox(discr[2], (bool *)&walk.f_bAllbonuses);
				ImGui::Checkbox(discr[3], (bool *)&walk.f_bSavePointsEnabled);
				ImGui::Checkbox(discr[4], (bool *)&walk.f_bTripleJump);
				ImGui::SliderInt(discr[5], (int *)&walk.f_dwStartLives, 1, 4);
			} else {
				ImGui::Text("Description:");
				if (walk.f_bAirJumpDmg) ImGui::Text(discr[0]);
				if (walk.f_bAISnowballs) ImGui::Text(discr[1]);
				if (walk.f_bAllbonuses) ImGui::Text(discr[2]);
				if (walk.f_bSavePointsEnabled) ImGui::Text(discr[3]);
				if (walk.f_bTripleJump) ImGui::Text(discr[4]);
				ImGui::Text(discr[5], walk.f_dwStartLives);
				ImGui::Text(discr[6], walk.f_fLevelTime);
			}
			ImGui::Spacing();
			ImGui::Text("Score multipliers:");
			ImGui::Text("Time bonus: %.3f");
			ImGui::Text("Present: %.3f");
			ImGui::Text("Hardcore bonus: %.3f");
		}

		if (ImGui::CollapsingHeader("Graphics")) {

		}

		if (ImGui::CollapsingHeader("Audio")) {
			static float master_volume = 1.0f,
			sfx_volume = 1.0f, music_volume = 1.0f;
			ImGui::SliderFloat("Master volume", &master_volume, 0.0, 1.0f, "%.2f");
			ImGui::SliderFloat("SFX volume", &sfx_volume, 0.0, 1.0f, "%.2f");
			ImGui::SliderFloat("Music volume", &music_volume, 0.0, 1.0f, "%.2f");
		}

		if (ImGui::CollapsingHeader("Controls")) {
			static bool mouse, pad;
			ImGui::Checkbox("Mouse controls", &mouse);
			ImGui::SameLine(265.0f); /* TODO: Автоматическое вычисление этого прикола */
			ImGui::Checkbox("Gamepad controls", &pad);
			ImGui::SetCursorPosX((wsize.x - 130.0f) * 0.5f);
			ImGui::Text("Keyboard bindings"); /* TODO: И этого тоже */
		}

		ImGui::End();
	}

private:
	bool m_bShowSettings = true;
};

Menu::Menu() {
	m_lpPlayer = new Player;
	m_lpMenuCtl = new MenuController;

	auto saves = new MenuButtons("Saved games");
	saves->AddButton("Work in progress", [](MenuBase *) {});
	saves->AddCloseButton();
	m_lpMenuCtl->AddMenu(saves);

	auto options = new MenuOptions("Options");
	m_lpMenuCtl->AddMenu(options);

	auto hscores = new MenuButtons("Highscores");
	hscores->AddButton("Work in progress", [](MenuBase *) {});
	hscores->AddCloseButton();
	m_lpMenuCtl->AddMenu(hscores);

	auto main = new MenuButtons("Main menu", true);
	main->AddButton("Load game", saves);
	main->AddButton("Start new game", [](MenuBase *) {
		Engine::GetInstance()->SetRunner(Engine::GAME);
	});
	main->AddButton("Options", options);
	main->AddButton("Highscores", hscores);
	main->AddButton("Quit game", [](MenuBase *) {
		::PostQuitMessage(0);
	});
	m_lpMenuCtl->AddMenu(main);
	m_lpMenuCtl->ShowMenu(main);
}

Menu::~Menu() {
	delete m_lpPlayer;
	delete m_lpMenuCtl;
}

VOID Menu::OnOpen(DWORD) {
	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();
	auto camera = graphics->GetCamera();
	auto level = engine->SysLevel();
	auto pobj = m_lpPlayer->GetDrawObject();
	EASSERT(level->Load("levels\\100.dat"));
	camera->SetFollow(&pobj->f_vPos, &m_vCamRot);
	camera->f_fFollowHeightMult = 0.35f;
	pobj->f_vPos.y = 0.0f;
}

VOID Menu::OnClose() {
	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();
	auto camera = graphics->GetCamera();
	camera->f_fFollowHeightMult = 0.5f;
	camera->SetFollow();
}

VOID Menu::OnUpdate(FLOAT delta) {
	static FLOAT timer = 0.0f;
	m_vCamRot.y = D3DXToRadian(77.5f) + std::sinf(timer) * D3DXToRadian(22.5f);
	timer += delta;
}

VOID Menu::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}

VOID Menu::OnDrawUI() {
	m_lpMenuCtl->Draw();
}
