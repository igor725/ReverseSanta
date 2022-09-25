#pragma once

#include <Windows.h>
#include "baserunner.hh"
#include "game\player.hh"
#include "menuctl.hh"

class Menu : public BaseRunner {
public:
	Menu();
	~Menu();

	VOID OnOpen(DWORD prev);
	VOID OnClose();

	LRESULT OnWndProc(HWND, UINT, WPARAM, LPARAM) {
		/* Запрещаем игре передавать события нажатий дальше по коду, оставляя их на совести imgui */
		return true;
	}
	VOID OnUpdate(FLOAT delta);
	VOID OnDraw(LPDIRECT3DDEVICE9 device);
	VOID OnDrawUI();

private:
	Player *m_lpPlayer = nullptr;
	MenuController *m_lpMenuCtl;

	D3DXVECTOR3 m_vCamRot = {0.0f, 0.0f, 0.0f};
};
