#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <dinput.h>

struct InputState {
	DIMOUSESTATE f_msMouseNew = {0};
	DIMOUSESTATE f_msMousePrev = {0};
	BYTE f_lpKeyboard[256] = {0};

	// Keyboard
	inline BYTE Key(DWORD key) { return f_lpKeyboard[key]; }

	// Mouse
	inline LONG CurCX() { return f_msMouseNew.lX; }
	inline LONG CurCY() { return f_msMouseNew.lY; }
	inline LONG CurLX() { return f_msMousePrev.lX; }
	inline LONG CurLY() { return f_msMousePrev.lY; }
	inline BOOL CurMoved() {
		return (f_msMouseNew.lX != f_msMousePrev.lX) || (f_msMouseNew.lY != f_msMousePrev.lY);
	}
	inline BOOL MBIsDown(BYTE idx) {
		return (f_msMouseNew.rgbButtons[idx] & 0x80) != 0x00;
	}
	inline BOOL MBChanged(BYTE idx) {
		return f_msMousePrev.rgbButtons[idx] != f_msMouseNew.rgbButtons[idx];
	}
	inline BOOL LeftClick() {
		return MBChanged(0) && !MBIsDown(0);
	}
};

class BaseRunner {
public:
	virtual void OnOpen() {}
	virtual void OnClose() {}

	virtual LRESULT OnWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
		(void)hWnd, iMsg, wParam, lParam;
		return (LRESULT)0;
	}
	virtual void OnInput(FLOAT, InputState *) {}
	virtual void OnUpdate(FLOAT) {}
	virtual void OnDraw(LPDIRECT3DDEVICE9) {}
	virtual void OnDrawUI() {}

	virtual void OnDeviceLost() {}
	virtual void OnDeviceReset(LPDIRECT3DDEVICE9) {}
};
