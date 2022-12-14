#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "camera.hh"

class Graphics {
public:
	Graphics(HINSTANCE hInst);
	~Graphics() { Shutdown(); }

	VOID UpdateWindow(BOOL state, INT width, INT height);

	VOID Shutdown();
	VOID RecreateDevice();
	VOID UpdateLight();
	VOID EnableLighting(BOOL state = true);

	inline D3DLIGHT9 *GetLight() { return m_bLightEnabled ? &m_Light : nullptr; }
	inline LPDIRECT3DDEVICE9 GetDevice() { return m_lpDevice; }
	inline Camera *GetCamera() { return m_lpCamera; }
	inline HWND GetWindow() { return m_hWindow; }

	BOOL TestDevice();
	LPDIRECT3DDEVICE9 BeginFrame(FLOAT delta);
	VOID StartUI();
	VOID EndUI();
	VOID EndFrame();
	VOID PresentFrame();
	LPDIRECT3DSURFACE9 PresentToSurface();

	inline VOID Show() { ShowWindow(m_hWindow, SW_NORMAL); }

private:
	const DWORD m_dwD3DDeviceFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING |
	D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE | D3DCREATE_NOWINDOWCHANGES;
	const DWORD m_dwD3DClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

	BOOL m_bDeviceLost = false,
	m_bDeviceOccluded = false,
	m_bLightEnabled = false,
	m_bRenderSuccededTwice = false,
	m_bRenderPossibleFatal = false;

	HWND m_hWindow = nullptr;
	LPDIRECT3D9 m_lpD3D = nullptr;
	LPDIRECT3D9EX m_lpD3DEx = nullptr;
	LPDIRECT3DDEVICE9 m_lpDevice = nullptr;
	LPDIRECT3DDEVICE9EX m_lpDeviceEx = nullptr;
	LPDIRECT3DTEXTURE9 m_lpTexture = nullptr;
	D3DPRESENT_PARAMETERS m_D3DPresent = {};
	D3DLIGHT9 m_Light = {};

	Camera *m_lpCamera = nullptr;

	VOID ResetPresentParams(DWORD width = 0, DWORD height = 0) {
		ZeroMemory(&m_D3DPresent, sizeof(D3DPRESENT_PARAMETERS));
		m_D3DPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_D3DPresent.EnableAutoDepthStencil = true;
		m_D3DPresent.hDeviceWindow = m_hWindow;
		m_D3DPresent.BackBufferCount = 1;
		m_D3DPresent.BackBufferWidth = width;
		m_D3DPresent.BackBufferHeight = height;
		m_D3DPresent.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_D3DPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		m_D3DPresent.Windowed = true;
	}
};
