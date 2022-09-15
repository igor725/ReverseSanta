#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "camera.hh"

class Graphics {
public:
	Graphics(HINSTANCE hInst);
	~Graphics();

	void UpdateLight();
	void EnableLightning(bool state = true);

	inline D3DLIGHT9 *GetLight() { return m_bLightEnabled ? &m_Light : nullptr; }
	inline LPDIRECT3DDEVICE9 GetDevice() { return m_lpDevice; }
	inline Camera *GetCamera() { return m_lpCamera; }
	inline HWND GetWindow() { return m_hWindow; }

	bool TestDevice();
	LPDIRECT3DDEVICE9 BeginFrame(FLOAT delta);
	void EndFrame();
	void PresentFrame();
	LPDIRECT3DSURFACE9 PresentToSurface();

	inline void Show() { ShowWindow(m_hWindow, SW_NORMAL); }

private:
	const DWORD m_dwD3DDeviceFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING |
	D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE | D3DCREATE_NOWINDOWCHANGES;
	const DWORD m_dwD3DClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
	const DWORD m_dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	bool m_bDeviceLost = false,
	m_bDeviceOccluded = false,
	m_bLightEnabled = false;

	HWND m_hWindow = nullptr;
	LPDIRECT3D9 m_lpD3D = nullptr;
	LPDIRECT3D9EX m_lpD3DEx = nullptr;
	LPDIRECT3DDEVICE9 m_lpDevice = nullptr;
	LPDIRECT3DDEVICE9EX m_lpDeviceEx = nullptr;
	LPDIRECT3DTEXTURE9 m_lpTexture = nullptr;
	D3DPRESENT_PARAMETERS m_D3DPresent = {};
	D3DLIGHT9 m_Light = {};

	Camera *m_lpCamera = nullptr;

	void ResetPresentParams() {
		ZeroMemory(&m_D3DPresent, sizeof(D3DPRESENT_PARAMETERS));
		m_D3DPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_D3DPresent.EnableAutoDepthStencil = true;
		m_D3DPresent.hDeviceWindow = m_hWindow;
		m_D3DPresent.BackBufferFormat = D3DFMT_UNKNOWN;
		m_D3DPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		m_D3DPresent.Windowed = true;
	}
};
