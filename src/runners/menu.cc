#include "engine.hh"
#include "exceptions.hh"
#include "menu.hh"
#include "imgui.h"

#define DWND_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse

Menu::~Menu() {
	delete m_lpPlayer;
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
	auto &io = ImGui::GetIO();
	auto engine = Engine::GetInstance();
	static const ImVec2 btnsz = ImVec2(150, 0);
	static const ImVec2 wsize = ImVec2(410, 260);
	const ImVec2 wcenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

	float crb = (wsize.x - btnsz.x) * 0.5f;
	static bool showsettings = false,
	showscores = false, showloads = false;

	if (showsettings) {
		ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Settings", &showsettings, DWND_FLAGS);

		if (ImGui::CollapsingHeader("Diffulty")) {
			auto &walk = engine->SysWalkthrough()->GetConfig();
			static const char *diffs[] = {"Easy", "Normal", "Hard", "Very hard", "Custom"};
			static const char *discr[] = {
				"Each air jump takes 5 seconds off the timer",
				"Enemies throwing snowballs",
				"Collect all presents to be able to finish the level",
				"Extra lives and savepoints are available",
				"Allow double air jump"
			};
			static int current = 0;
			if (ImGui::ListBox("Select difficulty", &current, diffs, IM_ARRAYSIZE(diffs))) {
				// if (current < 4) walk = GetDifficultyPreset(current);
			}

			ImGui::Spacing();
			if (current == 4) {
				ImGui::Checkbox(discr[0], (bool *)&walk.f_bAirJumpDmg);
				ImGui::Checkbox(discr[1], (bool *)&walk.f_bAISnowballs);
				ImGui::Checkbox(discr[2], (bool *)&walk.f_bAllbonuses);
				ImGui::Checkbox(discr[3], (bool *)&walk.f_bSavePointsEnabled);
				ImGui::Checkbox(discr[4], (bool *)&walk.f_bTripleJump);
			} else {
				ImGui::Text("Description:");
				if (walk.f_bAirJumpDmg) ImGui::Text(discr[0]);
				if (walk.f_bAISnowballs) ImGui::Text(discr[1]);
				if (walk.f_bAllbonuses) ImGui::Text(discr[2]);
				if (walk.f_bSavePointsEnabled) ImGui::Text(discr[3]);
				if (walk.f_bTripleJump) ImGui::Text(discr[4]);
			}
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
		return;
	}

	if (showscores) {
		ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Highscores", &showscores, DWND_FLAGS);

		ImGui::End();
		return;
	}

	if (showloads) {
		ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Saved games", &showloads, DWND_FLAGS);

		ImGui::End();
		return;
	}

	ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Main menu", nullptr, DWND_FLAGS);

	ImGui::SetCursorPos(ImVec2(crb, (wsize.y - 92.0f) * 0.5f));
	if (ImGui::Button("Load game", btnsz))
		showloads = !showloads;
	ImGui::SetCursorPosX(crb);
	if (ImGui::Button("Start new game", btnsz))
		engine->SetRunner(Engine::GAME);
	ImGui::SetCursorPosX(crb);
	if (ImGui::Button("Options", btnsz))
		showsettings = !showsettings;
	ImGui::SetCursorPosX(crb);
	if (ImGui::Button("Highscores", btnsz))
		showscores = !showscores;
	ImGui::SetCursorPosX(crb);
	if (ImGui::Button("Quit game", btnsz))
		::PostQuitMessage(0);

	ImGui::End();
}
