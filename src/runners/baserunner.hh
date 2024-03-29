#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <Windows.h>
#include <d3d9.h>
#include <dinput.h>

#include "exceptions.hh"

struct InputState {
	DIMOUSESTATE f_msMouseNew = {0};
	DIMOUSESTATE f_msMousePrev = {0};
	BYTE f_lpKeyboardNew[256] = {0};
	BYTE f_lpKeyboardOld[256] = {0};

	// State
	inline VOID Begin(LPDIRECTINPUTDEVICE8 mouse, LPDIRECTINPUTDEVICE8 keyboard) {
		if (auto hRes = mouse->GetDeviceState(sizeof(f_msMouseNew), &f_msMouseNew))
			throw DIException(hRes, EAFINFO);
		if (auto hRes = keyboard->GetDeviceState(sizeof(f_lpKeyboardNew), &f_lpKeyboardNew))
			throw DIException(hRes, EAFINFO);
	}
	inline VOID End() {
		f_msMousePrev = f_msMouseNew;
		for (WORD i = 0; i < 256; i++)
			f_lpKeyboardOld[i] = f_lpKeyboardNew[i];
	}

	// Keyboard
	inline BOOL KBIsDown(BYTE key) { return (f_lpKeyboardNew[key] & 0x80) != 0x00; }
	inline BOOL KBChanged(BYTE key) { return f_lpKeyboardNew[key] != f_lpKeyboardOld[key]; }
	inline BOOL KBIsJustUp(BYTE key) { return KBChanged(key) && !KBIsDown(key); }
	inline BOOL KBIsJustDown(BYTE key) { return KBChanged(key) && KBIsDown(key); }

	// Mouse
	inline LONG CurCX() { return f_msMouseNew.lX; }
	inline LONG CurCY() { return f_msMouseNew.lY; }
	inline LONG CurLX() { return f_msMousePrev.lX; }
	inline LONG CurLY() { return f_msMousePrev.lY; }
	inline BOOL CurMoved() { return (f_msMouseNew.lX != f_msMousePrev.lX) || (f_msMouseNew.lY != f_msMousePrev.lY); }
	inline BOOL MBIsDown(BYTE key) { return (f_msMouseNew.rgbButtons[key] & 0x80) != 0x00; }
	inline BOOL MBChanged(BYTE key) { return f_msMousePrev.rgbButtons[key] != f_msMouseNew.rgbButtons[key]; }
	inline BOOL MBIsJustUp(BYTE key) { return MBChanged(key) && !MBIsDown(key); }
	inline BOOL MBIsJustDown(BYTE key) { return MBChanged(key) && MBIsDown(key); }
};

class BaseRunner {
public:
	virtual VOID OnOpen(DWORD) {}
	virtual VOID OnClose() {}
	virtual VOID OnPause(BOOL) {}

	virtual LRESULT OnWndProc(HWND, UINT, WPARAM, LPARAM) { return (LRESULT)0; }
	virtual VOID OnInput(FLOAT, InputState *) {}
	virtual VOID OnUpdate(FLOAT) {}
	virtual VOID OnDraw(LPDIRECT3DDEVICE9) {}
	virtual VOID OnDrawUI() {}

	virtual VOID OnDeviceLost() {}
	virtual VOID OnDeviceReset(LPDIRECT3DDEVICE9) {}
};
