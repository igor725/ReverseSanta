#pragma once

#include "direct_input.hh"
#include "base_runner.hh"

class Input {
public:
	Input(HINSTANCE hInst, HWND hWnd);

	bool Capture();
	bool Release();

	void Update(FLOAT delta, BaseRunner *runner);

private:
	LPDIRECTINPUT8 m_lpDI = nullptr;
	LPDIRECTINPUTDEVICE8 m_lpDIK = nullptr, m_lpDIM = nullptr;
	bool m_bInputAcquired = false;
};
