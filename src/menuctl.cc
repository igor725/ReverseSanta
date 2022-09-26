#include "menuctl.hh"
#include "imgui.h"

#define DWND_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse

void MenuButtons::Draw() {
	auto &io = ImGui::GetIO();
	const ImVec2 btnsz = ImVec2(150, 0);
	const ImVec2 wsize = ImVec2(404, 260);
	const ImVec2 wcenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	const float crb = (wsize.x - btnsz.x) * 0.5f;

	ImGui::SetNextWindowSize(wsize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(wcenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin(m_sTitle.c_str(), nullptr, DWND_FLAGS | (m_bHideTitle ? ImGuiWindowFlags_NoTitleBar : 0));

	if (auto cnt = m_vItems.size()) {
		ImGui::SetCursorPosY((wsize.y - cnt * ImGui::GetFrameHeightWithSpacing()) * 0.5f);

		for (auto &item : m_vItems) {
			ImGui::SetCursorPosX(crb);
			if (ImGui::Button(item.f_sTitle.c_str(), btnsz)) {
				if (item.f_eType == CALLBACK) 
					item.f_uData.f_lpCallback(this);
				else if (item.f_eType == SUBMENU)
					GetController()->ShowMenu(item.f_uData.f_lpSubMenu);
				else if (item.f_eType == CLOSE)
					GetController()->CloseMenu();
			}
		}
	}

	ImGui::End();
}
