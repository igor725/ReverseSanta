#include "engine.hh"
#include "graphics.hh"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	auto engine = Engine::GetInstance();

	if (auto runner = engine->GetRunner())
		if (runner->OnWndProc(hWnd, iMsg, wParam, lParam))
			return true;

	switch (iMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_RBUTTONUP:
			engine->SysInput()->Capture();
			break;
		case WM_KEYUP:
			if(wParam == VK_ESCAPE)
				if (!engine->SysInput()->Release())
					PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case WM_ACTIVATE:
			if(wParam == WA_INACTIVE)
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
	if (FAILED(RegisterClass(&wc))) ExitProcess(1);

	RECT winrect = {0, 0, 800, 600};
	AdjustWindowRect(&winrect, m_dwStyle, FALSE);

	INT width = winrect.right - winrect.left,
	height = winrect.bottom - winrect.top;

	INT cx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
	cy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	m_hWindow = CreateWindow(wc.lpszClassName, L"D3D Window",
		m_dwStyle, cx, cy, width, height, NULL, NULL, hInst, this);
	ResetPresentParams();
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

		if (m_lpD3DEx) m_lpD3DEx->Release();
		if (m_lpDeviceEx) m_lpDeviceEx->Release();
		m_lpDevice = nullptr, m_lpDeviceEx = nullptr, m_lpD3DEx = nullptr;
	} while(0);

	if (m_lpDevice == nullptr) {
		if ((m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
			ExitProcess(1);
		if (m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow,
		m_dwD3DDeviceFlags, &m_D3DPresent, &m_lpDevice) != D3D_OK)
			ExitProcess(1);
	}

	FLOAT aspect = m_D3DPresent.BackBufferWidth / (FLOAT)m_D3DPresent.BackBufferHeight;
	m_lpCamera = new Camera(D3DX_PI / 4.0f, aspect, 0.01f, 10000.0f);
}

LPDIRECT3DDEVICE9 Graphics::BeginFrame() {
	if (!m_lpDevice) return nullptr;
	m_lpCamera->Update();
	m_lpDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_lpDevice->SetTransform(D3DTS_PROJECTION, m_lpCamera->GetProjection());
	m_lpDevice->SetTransform(D3DTS_VIEW, m_lpCamera->GetView());

	if (m_lpDevice->BeginScene() == D3D_OK) {
		m_lpDevice->Clear(0, nullptr, m_dwD3DClearFlags, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		return m_lpDevice;
	}

	return nullptr;
}

void Graphics::EndFrame() {
	m_lpDevice->EndScene();
}

void Graphics::PresentFrame() {
	HRESULT hRes;
	if (m_lpDeviceEx)
		hRes = m_lpDeviceEx->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
	else
		hRes = m_lpDevice->Present(nullptr, nullptr, nullptr, nullptr);

	if (hRes != D3D_OK) ExitProcess(1);
}
