#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "runners\baserunner.hh"

class Input {
public:
	Input(HINSTANCE hInst, HWND hWnd);

	BOOL Capture();
	BOOL Release();

	VOID Update(FLOAT delta, BaseRunner *runner);

private:
	LPDIRECTINPUT8 m_lpDI = nullptr;
	LPDIRECTINPUTDEVICE8 m_lpDIK = nullptr, m_lpDIM = nullptr;
	BOOL m_bInputAcquired = false;
};
