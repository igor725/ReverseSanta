#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

#include "camera.hh"

class Graphics {
public:
	Graphics(HINSTANCE hInst);

	inline LPDIRECT3DDEVICE9 GetDevice() { return m_lpDevice; }
	inline Camera *GetCamera() { return m_lpCamera; }
	inline HWND GetWindow() { return m_hWindow; }

	LPDIRECT3DDEVICE9 BeginFrame();
	void EndFrame();
	void PresentFrame();

	inline void Show() { ShowWindow(m_hWindow, SW_NORMAL); }

private:
	const DWORD m_dwD3DDeviceFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING |
	D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE | D3DCREATE_NOWINDOWCHANGES;
	const DWORD m_dwD3DClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
	const DWORD m_dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	HWND m_hWindow = nullptr;
	LPDIRECT3D9 m_lpD3D = nullptr;
	LPDIRECT3D9EX m_lpD3DEx = nullptr;
	LPDIRECT3DDEVICE9 m_lpDevice = nullptr;
	LPDIRECT3DDEVICE9EX m_lpDeviceEx = nullptr;
	LPD3DXFONT m_lpFont = nullptr;
	D3DPRESENT_PARAMETERS m_D3DPresent = {0};

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
