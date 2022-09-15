#include "engine.hh"
#include "graphics.hh"
#include "exceptions.hh"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	if (iMsg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	auto engine = Engine::GetInstance();
	if (!engine) return DefWindowProc(hWnd, iMsg, wParam, lParam);

	if (auto runner = engine->GetRunner())
		if (runner->OnWndProc(hWnd, iMsg, wParam, lParam))
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
	if (FAILED(RegisterClass(&wc))) ExitProcess(1);

	RECT winrect = {0, 0, 800, 600};
	AdjustWindowRect(&winrect, m_dwStyle, FALSE);

	INT width = winrect.right - winrect.left,
	height = winrect.bottom - winrect.top;

	INT cx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
	cy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	m_hWindow = CreateWindow(wc.lpszClassName, L"D3D9 Window",
	m_dwStyle, cx, cy, width, height, NULL, NULL, hInst, this);
	ResetPresentParams();

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

	FLOAT aspect = m_D3DPresent.BackBufferWidth / (FLOAT)m_D3DPresent.BackBufferHeight;
	m_lpCamera = new Camera(D3DX_PI / 4.0f, aspect, 0.01f, 10000.0f);

	DASSERT(D3DXCreateTexture(m_lpDevice, 400, (UINT)(400 / aspect), D3DX_DEFAULT,
	D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_lpTexture));

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

	// D3DCAPS9 d3dcaps;
	// if (m_lpDevice->GetDeviceCaps(&d3dcaps) == D3D_OK)
	// 	std::cerr << d3dcaps. << std::endl;
}

Graphics::~Graphics() {
	if (m_lpDeviceEx) m_lpDeviceEx->Release();
	else m_lpDevice->Release();
	if (m_lpTexture) m_lpTexture->Release();
	if (m_lpD3D) m_lpD3D->Release();
	delete m_lpCamera;

	m_lpDevice = nullptr, m_lpDeviceEx = nullptr;
}

void Graphics::UpdateLight() {
	DASSERT(m_lpDevice->SetLight(0, &m_Light));
}

void Graphics::EnableLightning(bool state) {
	DASSERT(m_lpDevice->LightEnable(0, state));
	m_bLightEnabled = state;
}

bool Graphics::TestDevice() {
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

	switch (auto ret = m_lpDevice->TestCooperativeLevel()) {
		case D3D_OK:
			m_bDeviceLost = false;
			break;
		case D3DERR_DEVICELOST:
			m_bDeviceLost = true;
			Sleep(200);
			return false;
		case D3DERR_DEVICENOTRESET:
			ResetPresentParams();
			BaseRunner *runner;
			if ((runner = Engine::GetInstance()->GetRunner()) != nullptr)
				runner->OnDeviceLost(m_lpDevice);

			if (m_lpDeviceEx)
				DASSERT(m_lpDeviceEx->ResetEx(&m_D3DPresent, nullptr));
			else
				DASSERT(m_lpDevice->Reset(&m_D3DPresent));

			if (runner) runner->OnDeviceReset(m_lpDevice);
			break;

		default: DASSERT(ret);
	}

	return true;
}

LPDIRECT3DDEVICE9 Graphics::BeginFrame(FLOAT delta) {
	m_lpCamera->Update(delta);

	m_lpDevice->SetRenderState(D3DRS_LIGHTING, m_bLightEnabled);
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
	
	if (hRes == S_PRESENT_OCCLUDED) {
		m_bDeviceOccluded = true;
		return;
	} else if (hRes == S_PRESENT_MODE_CHANGED) {
		m_bDeviceOccluded = false;
		return;
	}

	DASSERT(hRes);
}

LPDIRECT3DSURFACE9 Graphics::PresentToSurface() {
	LPDIRECT3DSURFACE9 rt, sf;
	m_lpTexture->GetSurfaceLevel(0, &sf);
	m_lpDevice->GetRenderTarget(0, &rt);
	m_lpDevice->StretchRect(rt, nullptr, sf, nullptr, D3DTEXF_NONE);
	rt->Release();
	return sf;
}
