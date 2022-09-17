#include "engine.hh"
#include "editor.hh"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Editor::Editor() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();
	auto window = graphics->GetWindow();
	auto device = graphics->GetDevice();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

LRESULT Editor::OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	auto engine = Engine::GetInstance();
	if (iMsg == WM_KEYUP) {
		if (wParam == VK_F1) {
			engine->SysInput()->Release();
			m_Menu.Toggle();
			return true;
		}
	}

	if (auto res = ImGui_ImplWin32_WndProcHandler(hWnd, iMsg, wParam, lParam))
		return res;

	ImGuiIO &io = ImGui::GetIO();
	auto ongui = io.WantCaptureMouse || io.WantCaptureKeyboard;
	if (!ongui && iMsg == WM_LBUTTONUP) {
		auto x = (SHORT)(((DWORD_PTR)lParam) & 0xFFFF),
		y = (SHORT)(((DWORD_PTR)lParam >> 16) & 0xFFFF);
		engine->GetObjectOn(&m_Menu.m_odPicked, x, y);
	}

	return ongui;
}

void Editor::OnDeviceLost() {
	ImGui_ImplDX9_Shutdown();
}

void Editor::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	ImGui_ImplDX9_Init(device);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Editor::OnInput(FLOAT delta, InputState *state) {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto speed = 15.0f * delta;

	if (state->Key(DIK_LSHIFT) & 0x80)
		speed += 30.0f * delta;

	auto fwd = camera->GetForward() * speed;

	if (state->Key(DIK_W) & 0x80)
		camera->f_vEye += fwd;
	else if (state->Key(DIK_S) & 0x80)
		camera->f_vEye -= fwd;

	if (state->Key(DIK_A) & 0x80)
		camera->f_vEye.x -= fwd.z, camera->f_vEye.z += fwd.x;
	else if (state->Key(DIK_D) & 0x80)
		camera->f_vEye.x += fwd.z, camera->f_vEye.z -= fwd.x;

	if (state->CurMoved())
		camera->f_vRot += D3DXVECTOR3{state->CurCX() * 0.001f, state->CurCY() * 0.001f, 0.0f};

	if (state->LeftClick() && engine->GetObjectOn(&m_Menu.m_odPicked))
		engine->SysInput()->Release();
}

void Editor::OnUpdate(FLOAT) {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_Menu.Draw();

	ImGui::EndFrame();
}

void Editor::OnDraw(LPDIRECT3DDEVICE9 device) {
	(void)device;
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
