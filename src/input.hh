#pragma once

#include "direct_input.hh"
#include "base_runner.hh"

class Input {
public:
	Input(HINSTANCE hInst, HWND hWnd);

	BOOL Capture();
	BOOL Release();

	void Update(FLOAT delta, BaseRunner *runner);

private:
	LPDIRECTINPUT8 m_lpDI = nullptr;
	LPDIRECTINPUTDEVICE8 m_lpDIK = nullptr, m_lpDIM = nullptr;
	BOOL m_bInputAcquired = false;
};
