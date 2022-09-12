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
	if (auto res = ImGui_ImplWin32_WndProcHandler(hWnd, iMsg, wParam, lParam))
		return res;

	ImGuiIO &io = ImGui::GetIO();
	return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

void Editor::OnInput(FLOAT delta, InputState *state) {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto eye = camera->GetEye();
	auto speed = 15.0f * delta;

	if (state->Key(DIK_LSHIFT) & 0x80)
		speed += 30.0f * delta;

	D3DXVECTOR3 fwd = {
		std::sinf(camera->GetYaw()) * std::cosf(camera->GetPitch())  * speed,
		std::sinf(-camera->GetPitch()) * speed,
		std::cosf(camera->GetYaw()) * std::cosf(camera->GetPitch()) * speed
	};

	if (state->Key(DIK_W) & 0x80)
		*eye += fwd;
	else if (state->Key(DIK_S) & 0x80)
		*eye -= fwd;

	if (state->Key(DIK_A) & 0x80)
		eye->x -= fwd.z, eye->z += fwd.x;
	else if (state->Key(DIK_D) & 0x80)
		eye->x += fwd.z, eye->z -= fwd.x;

	if (state->CurMoved())
		camera->Move(state->CurCX() * 0.001f, state->CurCY() * 0.001f);
}

void Editor::OnUpdate(FLOAT delta) {(void)delta;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show = true;

	if (show)
		ImGui::ShowDemoWindow(&show);

	ImGui::EndFrame();
}

void Editor::OnDraw(LPDIRECT3DDEVICE9 device) {
	(void)device;
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
