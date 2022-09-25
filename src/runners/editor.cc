#include "engine.hh"
#include "editor.hh"
#include "imgui.h"

LRESULT Editor::OnWndProc(HWND, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	auto engine = Engine::GetInstance();
	if (iMsg == WM_KEYUP) {
		if (wParam == VK_F1) {
			engine->SetPause(true);
			m_Menu.Toggle();
			return true;
		} if (wParam == VK_ESCAPE && engine->IsPaused()) {
			engine->SetRunner(Engine::GAME);
			engine->SetPause(false);
			return true;
		}
	}  if (iMsg == WM_LBUTTONUP && !ImGui::GetIO().WantCaptureMouse) {
		auto x = (SHORT)(((DWORD_PTR)lParam) & 0xFFFF),
		y = (SHORT)(((DWORD_PTR)lParam >> 16) & 0xFFFF);
		engine->GetObjectOn(&m_Menu.m_odPicked, x, y);
		return true;
	}

	return false;
}

VOID Editor::OnInput(FLOAT delta, InputState *state) {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto speed = 15.0f * delta;

	if (state->KBIsDown(DIK_LSHIFT))
		speed += 30.0f * delta;

	auto fwd = camera->GetForward() * speed;

	if (state->KBIsDown(DIK_W))
		camera->f_vEye += fwd;
	else if (state->KBIsDown(DIK_S))
		camera->f_vEye -= fwd;

	if (state->KBIsDown(DIK_A))
		camera->f_vEye.x -= fwd.z, camera->f_vEye.z += fwd.x;
	else if (state->KBIsDown(DIK_D))
		camera->f_vEye.x += fwd.z, camera->f_vEye.z -= fwd.x;

	if (state->CurMoved())
		camera->f_vRot += D3DXVECTOR3{state->CurCX() * 0.001f, state->CurCY() * 0.001f, 0.0f};

	if (state->MBIsJustUp(0) && engine->GetObjectOn(&m_Menu.m_odPicked))
		engine->SetPause(true);
}

VOID Editor::OnDrawUI() {
	m_Menu.Draw();
}
