#include "engine.hh"
#include "graphics.hh"
#include "exceptions.hh"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	if (iMsg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	auto engine = Engine::GetInstance();
	if (!engine) return DefWindowProc(hWnd, iMsg, wParam, lParam);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, iMsg, wParam, lParam))
		return DefWindowProc(hWnd, iMsg, wParam, lParam);

	if (auto runner = engine->GetRunner())
		if (runner->OnWndProc(hWnd, iMsg, wParam, lParam))
			return DefWindowProc(hWnd, iMsg, wParam, lParam);

	auto &io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		return DefWindowProc(hWnd, iMsg, wParam, lParam);

	switch (iMsg) {
		case WM_RBUTTONUP:
			engine->SysInput()->Capture();
			break;
		case WM_KEYUP:
			switch (wParam) {
				case VK_ESCAPE:
					if (!engine->SysInput()->Release())
						PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case VK_F5:
					engine->NextRunner();
					break;
			}
			break;
		case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				engine->SysInput()->Release();
			break;

		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}

	return 0;
}

Graphics::Graphics(HINSTANCE hInst) {
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"D3DAPP";
	EASSERT(SUCCEEDED(RegisterClass(&wc)));

	RECT winrect = {0, 0, 1024, 768};
	AdjustWindowRect(&winrect, m_dwStyle, FALSE);

	INT width = winrect.right - winrect.left,
	height = winrect.bottom - winrect.top;

	INT cx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
	cy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	m_hWindow = CreateWindow(wc.lpszClassName, L"D3D9 Window",
	m_dwStyle, cx, cy, width, height, NULL, NULL, hInst, this);

	ImGui_ImplWin32_Init(m_hWindow);

	ResetPresentParams();
	RecreateDevice();
}

VOID Graphics::Shutdown() {
	if (m_lpDeviceEx) m_lpDeviceEx->Release();
	else if (m_lpDevice) m_lpDevice->Release();
	if (m_lpTexture) m_lpTexture->Release();
	if (m_lpD3DEx) m_lpD3DEx->Release();
	else if (m_lpD3D) m_lpD3D->Release();
	delete m_lpCamera;

	m_lpCamera = nullptr, m_lpTexture = nullptr,
	m_lpDevice = nullptr, m_lpDeviceEx = nullptr,
	m_lpD3D = nullptr, m_lpD3DEx = nullptr;
}

static VOID DeviceLostHandler() {
	ImGui_ImplDX9_Shutdown();
	Engine::GetInstance()->OnDeviceLost();
}

static VOID DeviceResetHandler(LPDIRECT3DDEVICE9 device) {
	ImGui_ImplDX9_Init(device);
	ImGui_ImplDX9_CreateDeviceObjects();
	Engine::GetInstance()->OnDeviceReset(device);
}

VOID Graphics::RecreateDevice() {
	Graphics::Shutdown();

#if !defined(D3D_DISABLE_9EX)
	do {
		typedef HRESULT(*D3D9CREX)(UINT, LPDIRECT3D9EX *);
		D3D9CREX lpexfunc = (D3D9CREX)GetProcAddress(GetModuleHandle(L"d3d9.dll"), "Direct3DCreate9Ex");
		if (lpexfunc) {
			if (lpexfunc(D3D_SDK_VERSION, &m_lpD3DEx) == D3D_OK) {
				if (m_lpD3DEx->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow,
				m_dwD3DDeviceFlags, &m_D3DPresent, NULL, &m_lpDeviceEx) == D3D_OK &&
				m_lpDeviceEx->QueryInterface(IID_IDirect3DDevice9, (LPVOID *)&m_lpDevice) == D3D_OK)
					break;
			}
		}

		ResetPresentParams();
		if (m_lpD3DEx) m_lpD3DEx->Release();
		if (m_lpDeviceEx) m_lpDeviceEx->Release();
		m_lpDevice = nullptr, m_lpDeviceEx = nullptr, m_lpD3DEx = nullptr;
	} while(0);
#endif

	if (m_lpDevice == nullptr) {
		EASSERT((m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION)) != nullptr);
		DASSERT(m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow,
		m_dwD3DDeviceFlags, &m_D3DPresent, &m_lpDevice));
	}

	auto width = m_D3DPresent.BackBufferWidth,
	height = m_D3DPresent.BackBufferHeight;
	m_lpCamera = new Camera(width / (FLOAT)height);

	DASSERT(D3DXCreateTexture(m_lpDevice, width, height, D3DX_DEFAULT,
	D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_lpTexture));
	DASSERT(m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
	DASSERT(m_lpDevice->SetRenderState(D3DRS_CLIPPING, true));

	ZeroMemory(&m_Light, sizeof(m_Light));
	m_Light.Type = D3DLIGHT_POINT;
	m_Light.Diffuse.r = 0.8f;
	m_Light.Diffuse.g = 0.8f;
	m_Light.Diffuse.b = 0.8f;
	m_Light.Ambient.r = 1.0f;
	m_Light.Ambient.g = 1.0f;
	m_Light.Ambient.b = 1.0f;
	m_Light.Specular.r = 1.0f;
	m_Light.Specular.g = 1.0f;
	m_Light.Specular.b = 1.0f;

	m_Light.Position.x = 0.0f;
	m_Light.Position.y = 15.0f;
	m_Light.Position.z = 0.0f;

	m_Light.Attenuation0 = 0.4f;
	m_Light.Attenuation1 = 0.3f;
	m_Light.Attenuation2 = 0.2f;
	m_Light.Range = 20.0f;
	EnableLighting(false);
	UpdateLight();

	ImGui_ImplDX9_Init(m_lpDevice);
}

VOID Graphics::UpdateLight() {
	DASSERT(m_lpDevice->SetLight(0, &m_Light));
}

VOID Graphics::EnableLighting(BOOL state) {
	m_bLightEnabled = state;
	DASSERT(m_lpDevice->LightEnable(0, state));
	DASSERT(m_lpDevice->SetRenderState(D3DRS_LIGHTING, state));
}

BOOL Graphics::TestDevice() {
	if (!m_lpDevice) return false;

#if !defined(D3D_DISABLE_9EX)
	if (m_bDeviceOccluded) {
		switch (auto hRes = m_lpDeviceEx->CheckDeviceState(m_hWindow)) {
			case S_PRESENT_OCCLUDED:
				Sleep(200);
				return false;
			case S_OK:
			case S_PRESENT_MODE_CHANGED:
				m_bDeviceOccluded = false;
				break;
			default: DASSERT(hRes);
		}
	}
#endif

	if (m_bDeviceLost) {
		switch (auto hRes = m_lpDevice->TestCooperativeLevel()) {
			case D3DERR_DEVICELOST:
				Sleep(500);
				break;

			case D3DERR_DEVICENOTRESET:
				ResetPresentParams();

				if (m_lpDeviceEx)
					DASSERT(m_lpDeviceEx->ResetEx(&m_D3DPresent, nullptr));
				else
					DASSERT(m_lpDevice->Reset(&m_D3DPresent));

			/* fallthrough */
			case D3D_OK:
				m_bDeviceLost = false;
				DeviceResetHandler(m_lpDevice);
				break;

			default: DASSERT(hRes);
		}

		return false;
	}

	return true;
}

LPDIRECT3DDEVICE9 Graphics::BeginFrame(FLOAT delta) {
	m_lpCamera->Update(delta);

	m_lpDevice->SetTransform(D3DTS_PROJECTION, &m_lpCamera->f_mxProj);
	m_lpDevice->SetTransform(D3DTS_VIEW, &m_lpCamera->f_mxView);

	if (m_lpDevice->BeginScene() == D3D_OK) {
		m_lpDevice->Clear(0, nullptr, m_dwD3DClearFlags, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		return m_lpDevice;
	}

	return nullptr;
}

VOID Graphics::StartUI() {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

VOID Graphics::EndUI() {
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

VOID Graphics::EndFrame() {
	m_lpDevice->EndScene();
}

VOID Graphics::PresentFrame() {
	HRESULT hRes;
	if (m_lpDeviceEx)
		hRes = m_lpDeviceEx->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
	else
		hRes = m_lpDevice->Present(nullptr, nullptr, nullptr, nullptr);

	/* D3DERR_DEVICELOST - 0x88760868 */

	if (m_bRenderPossibleFatal)
		DASSERT(hRes);

	switch (hRes) {
		case D3D_OK:
			if (!m_bRenderSuccededTwice) {
				m_bRenderSuccededTwice = true;
				m_bRenderPossibleFatal = false;
			}
			break;

		case D3DERR_DEVICELOST:
			DeviceLostHandler();
			if (m_bRenderSuccededTwice) {
				m_bDeviceLost = true;
				m_bRenderPossibleFatal = true;
				m_bRenderSuccededTwice = false;
			} else {
				m_bDeviceLost = false;
				RecreateDevice();
				Engine::GetInstance()->OnDeviceReset(m_lpDevice);
			}
			break;
		case S_PRESENT_OCCLUDED:
			m_bDeviceOccluded = true;
			break;
		case S_PRESENT_MODE_CHANGED:
			m_bDeviceOccluded = false;
			break;

		default: DASSERT(hRes);
	}
}

LPDIRECT3DSURFACE9 Graphics::PresentToSurface() {
	LPDIRECT3DSURFACE9 rt, sf;
	DASSERT(m_lpTexture->GetSurfaceLevel(0, &sf));
	DASSERT(m_lpDevice->GetRenderTarget(0, &rt));
	DASSERT(m_lpDevice->GetRenderTargetData(rt, sf));
	rt->Release();
	return sf;
}
