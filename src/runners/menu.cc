#include "engine.hh"
#include "exceptions.hh"
#include "menu.hh"
#include "imgui.h"

#define DWND_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse

class MenuOptions : public MenuBase {
public:
	MenuOptions(std::string title) : MenuBase(title) {
		auto engine = Engine::GetInstance();
		auto config = engine->SysConfig();
		m_iDifficulty = config->GetDifficulty();
		engine->SysWalkthrough()->GetConfig() = m_lpPresets[m_iDifficulty];
	}

	void Draw() {
		if (!m_bShowSettings) {
			GetController()->CloseMenu();
			m_bShowSettings = true;
			return;
		}

		auto &io = ImGui::GetIO();
		auto engine = Engine::GetInstance();
		auto config = engine->SysConfig();
		const ImVec2 wsize = ImVec2(404, 260); // Придумать, как вынести весь этот прикол отсюда
		const ImVec2 wcenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
		static bool needrestart = false;

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
				"Double air jump allowed"
			};

			if (ImGui::ListBox("Select difficulty", &m_iDifficulty, diffs, IM_ARRAYSIZE(diffs))) {
				if (m_iDifficulty < 4) {
					walk = m_lpPresets[m_iDifficulty];
					config->SetDifficulty(m_iDifficulty);
				}
			}

			ImGui::Spacing();
			if (m_iDifficulty == 4) {
				ImGui::Checkbox(discr[0], (bool *)&walk.f_bAirJumpDmg);
				ImGui::Checkbox(discr[1], (bool *)&walk.f_bAISnowballs);
				ImGui::Checkbox(discr[2], (bool *)&walk.f_bAllbonuses);
				ImGui::Checkbox(discr[3], (bool *)&walk.f_bSavePointsEnabled);
				ImGui::Checkbox(discr[4], (bool *)&walk.f_bTripleJump);
				ImGui::SliderInt("Available lives", (int *)&walk.f_dwStartLives, 1, 4);
				ImGui::SliderFloat("Time per level", &walk.f_fLevelTime, 120.0f, 600.0f);
			} else {
				ImGui::Text("Description:");
				if (walk.f_bAirJumpDmg) ImGui::Text(discr[0]);
				if (walk.f_bAISnowballs) ImGui::Text(discr[1]);
				if (walk.f_bAllbonuses) ImGui::Text(discr[2]);
				if (walk.f_bSavePointsEnabled) ImGui::Text(discr[3]);
				if (walk.f_bTripleJump) ImGui::Text(discr[4]);
				ImGui::Text("Available lives: %d", walk.f_dwStartLives);
				ImGui::Text("Time per level: %.2f sec", walk.f_fLevelTime);
			}
			ImGui::Spacing();
			ImGui::Text("Score multipliers:");
			ImGui::Text("Time bonus: %.3f");
			ImGui::Text("Present: %.3f");
			ImGui::Text("Hardcore bonus: %.3f");
		}

		if (ImGui::CollapsingHeader("General")) {
			char *buf = config->GetArchivePath();
			if (ImGui::InputText("Game archive path", buf, 32))
				config->f_flModified |= Config::General | Config::NeedRestart;
		}

		if (ImGui::CollapsingHeader("Graphics")) {
			bool vsync = config->GetVSync(),
			fs = config->GetBorderless();
			static int res = 0;
			ImGui::Combo("Resolution", &res, "NOTYETxIMPLEMENTED\0""1337x228\0""666x333\0");
			if (ImGui::Checkbox("VSync", &vsync))
				config->SetVSync(vsync);
			ImGui::SameLine();
			if (ImGui::Checkbox("Borderless", &fs))
				config->SetBorderless(fs);
		}

		if (ImGui::CollapsingHeader("Camera")) {
			float dist = config->GetRenderDistance(),
			fov = config->GetFOV();
			if (ImGui::SliderFloat("Render distance", &dist, 50.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
				config->SetRenderDistance(dist);
			if (ImGui::SliderAngle("FoV", &fov, 40.0f, 120.0f))
				config->SetFOV(fov);
		}

		if (ImGui::CollapsingHeader("Audio")) {
			float mavol = config->GetVolume(0),
			sfvol = config->GetVolume(1), muvol = config->GetVolume(2);
			static int current_device = 0;
			ImGui::Combo("Audio device", &current_device, "Not\0Yet\0Implemented\0");
			if (ImGui::SliderFloat("Master volume", &mavol, 0.0, 1.0f, "%.2f"))
				config->SetVolume(0, mavol);
			if (ImGui::SliderFloat("SFX volume", &sfvol, 0.0, 1.0f, "%.2f"))
				config->SetVolume(1, sfvol);
			if (ImGui::SliderFloat("Music volume", &muvol, 0.0, 1.0f, "%.2f"))
				config->SetVolume(2, muvol);
		}

		if (ImGui::CollapsingHeader("Controls")) {
			static bool mouse, pad;
			ImGui::Checkbox("Mouse controls", &mouse);
			ImGui::SameLine(265.0f); /* TODO: Автоматическое вычисление этого прикола */
			ImGui::Checkbox("Gamepad controls", &pad);
			ImGui::SetCursorPosX((wsize.x - 130.0f) * 0.5f);
			ImGui::Text("Keyboard bindings"); /* TODO: И этого тоже */
		}

		if (ImGui::Button("Apply changes"))
			needrestart = config->Update();
		ImGui::SameLine();
		if (ImGui::Button("Save & apply changes"))
			needrestart = config->Save();
		ImGui::SameLine();
		if (config->IsChanged(Config::Anything)) {
			if (ImGui::Button("Revert changes"))
				needrestart = config->Revert();
		} else {
			if (ImGui::Button("Reset to defaults"))
				needrestart = config->Reset();
		}

		if (needrestart) ImGui::Text("* To apply some of the changed settings you need to\n  restart the game!");
		ImGui::End();
	}

private:
	bool m_bShowSettings = true;
	int m_iDifficulty = 0;

	const Walkthrough::Config m_lpPresets[4] = {
		{4, 600.0f, true, false, true, false, false},
		{}, // Стандартные настройки сложности
		{2, 320.0f, false, true, false, true, false},
		{1, 200.0f, false, true, false, true, true},
	};
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
