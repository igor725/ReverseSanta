#include "engine.hh"
#include "emenu.hh"
#include "imgui.h"
#include "todo.hh"

VOID EditorMenu::DrawMainMenu() {
	const auto vp = ImGui::GetMainViewport();
	const auto res = ImVec2(410, 260);

	ImGui::SetNextWindowPos(ImVec2((vp->WorkSize.x - res.x) * 0.5f, (vp->WorkSize.y - res.y) * 0.5f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(res, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("ReverseSanta Editor Menu", &m_bMainActive, ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}

	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();
	auto camera = graphics->GetCamera();

	if (ImGui::CollapsingHeader("Level manager")) {
		static int curr_level = 0;
		TODO("Scan levels folder");
		static LPSTR items[] = {
			"000.dat", "001.dat", "002.dat", "003.dat", "004.dat", "005.dat",
			"006.dat", "007.dat", "008.dat", "009.dat", "010.dat", "100.dat",
			"demo.dat"
		};

		if (ImGui::ListBox("Pick a level", &curr_level, items, IM_ARRAYSIZE(items), 5)) {
			auto level = engine->SysLevel();
			level->Load("levels\\" + std::string(items[curr_level]));
		}
	}

	if (ImGui::CollapsingHeader("Camera manager")) {
		if (ImGui::Button("Teleport to start"))
			camera->f_vRot = camera->f_vEye = {0.0f, 0.0f, 0.0f};
	}

	if (ImGui::CollapsingHeader("Light manager")) {
		auto light = graphics->GetLight();
		static bool light_enabled = light != nullptr;
		if (ImGui::Checkbox("Lightning enabled", &light_enabled)) {
			graphics->EnableLighting(light_enabled);
			graphics->UpdateLight();
		}

		if (light) {
			static const char *const ltypes[] = {nullptr, "Point", "Spot", "Directional"},
			*currtype = ltypes[light->Type];
			bool update = false;
			if (ImGui::BeginCombo("Light type", currtype)) {
				for (auto i = 1; i < IM_ARRAYSIZE(ltypes); i++) {
					bool issel = (light->Type == i);
					if (ImGui::Selectable(ltypes[i], issel)) {
						currtype = ltypes[i];
						light->Type = (D3DLIGHTTYPE)i;
						update = true;
					}
				}
				ImGui::EndCombo();
			}
			update |= ImGui::ColorPicker3("Diffuse color", (float *)&light->Diffuse);
			ImGui::Spacing();
			update |= ImGui::ColorPicker3("Specular color", (float *)&light->Specular);
			ImGui::Spacing();
			update |= ImGui::ColorPicker3("Ambient color", (float *)&light->Ambient);
			ImGui::Spacing();
			if (light->Type != D3DLIGHT_POINT) {
				update |= ImGui::SliderFloat3("Direction", (float *)&light->Direction, -1.0f, 1.0f, "%.3f");
				ImGui::Spacing();
			}
			if (light->Type != D3DLIGHT_DIRECTIONAL) {
				if (ImGui::Button("Teleport to camera")) {
					light->Position = camera->f_vEye;
					light->Direction = camera->GetForward();
					update = true;
				}
				update |= ImGui::SliderFloat("Range", &light->Range, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				ImGui::Spacing();
			}
			if (light->Type == D3DLIGHT_SPOT) {
				update |= ImGui::SliderFloat("Fallof", &light->Falloff, 0.0, 1.0, "%.3f");
				ImGui::Spacing();
			}
			update |= ImGui::SliderFloat3("Attenuation", (float *)&light->Attenuation0, 0.0f, 1.0f);
			ImGui::Spacing();
			if (light->Type == D3DLIGHT_SPOT) {
				update |= ImGui::SliderFloat("Theta", &light->Theta, 0.0f, light->Phi, "%.3f");
				ImGui::Spacing();
				update |= ImGui::SliderFloat("Phi", &light->Phi, 0.0f, D3DX_PI, "%.3f");
			}

			if (update)
				graphics->UpdateLight();
		}
	}

	ImGui::End();
}

static VOID DrawEnemyStatus(Level::EnemyData *ed) {
	ImGui::Text("AI enabled: %s", ed->f_bEnabled ? "true" : "false");
	ImGui::Text("AI pattern: 0x%08x", ed->f_dwBehaviourHash);
	ImGui::Text("AI direction: %.2f, %.2f", ed->f_fFwdX, ed->f_fFwdZ);
	ImGui::Text("AI state: %d", ed->f_dwState);
}

static VOID DrawEnemyButtons(Level::EnemyData *ed) {
	if (ImGui::Button("Toogle AI"))
		ed->f_bEnabled ^= true;
}

VOID EditorMenu::DrawObjectMenu() {
	auto lvlobj = m_odPicked.f_lpLObj;
	auto dobj = m_odPicked.f_lpDObj;
	bool isactive = true;

	ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin(lvlobj->f_name, &isactive, ImGuiWindowFlags_NoResize)) {
		ImGui::End();
		return;
	}

	ImGui::Text("Rotation: %.2f deg", D3DXToDegree(dobj->f_vRot.y));
	auto type = dobj->f_lpElem->f_eType;
	auto ud = dobj->f_lpUserData;
	switch (type) {
		case Elems::ENEMY:
		case Elems::ELEVATORENEMY:
			DrawEnemyStatus((Level::EnemyData *)ud);
			break;
	}

	ImGui::Separator();
	if (ImGui::DragFloat3("Position", (float *)&dobj->f_vPos, 0.01f))
		dobj->f_bAlerted = true;

	if (ImGui::Button("Rotate"))
		dobj->f_vRot.y = std::fmodf(dobj->f_vRot.y + D3DX_PI * 0.5f, D3DX_PI * 2.0f), dobj->f_bAlerted = true;

	switch(type) {
		case Elems::ENEMY:
		case Elems::ELEVATORENEMY:
			ImGui::SameLine();
			DrawEnemyButtons(((Level::EnemyData *)ud));
			break;
	}
	ImGui::Separator();


	ImGui::End();
	if (!isactive)
		m_odPicked = {nullptr, nullptr};
}

VOID EditorMenu::Draw() {
	if (m_bMainActive)
		DrawMainMenu();
	if (m_odPicked.f_lpDObj)
		DrawObjectMenu();
}
