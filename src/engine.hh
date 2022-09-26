#pragma once

#include <Windows.h>
#include <vector>

#include "config.hh"
#include "graphics.hh"
#include "input.hh"
#include "virtfs.hh"
#include "level.hh"
#include "walkthrough.hh"
#include "runners\baserunner.hh"
#include "rescache.hh"

class Engine {
public:
	enum Runner {
		NONE,
		MENU,
		EDITOR,
		GAME,

		MAX_RUNNERS
	};

	static Engine *GetInstance();

	Engine(HINSTANCE hInst);
	~Engine();

	VOID OnDeviceLost();
	VOID OnDeviceReset(LPDIRECT3DDEVICE9 device);

	inline BaseRunner *GetRunner() { return m_eCurrentRunner ? m_lpRunners[m_eCurrentRunner] : nullptr; }
	inline BaseRunner *GetRunner(DWORD i) { return i < MAX_RUNNERS ? m_lpRunners[i] : nullptr; }
	VOID SetRunner(Runner num);

	VOID SetPause(BOOL state);
	VOID Step(FLOAT delta);

	inline BOOL IsPaused() { return m_bPaused; }

	inline Config *SysConfig() { return m_lpConfig; }
	inline VirtFs *SysVirtFs() { return m_lpVirtFs; }
	inline ResCache *SysCache() { return m_lpCache; }
	inline Graphics *SysGraphics() { return m_lpGraphics; }
	inline Input *SysInput() { return m_lpInput; }
	inline Level *SysLevel() { return m_lpLevel; }
	inline Walkthrough *SysWalkthrough() { return m_lpWalkthrough; }

	BOOL GetObjectOn(Level::ObjectData *data, DWORD x = (DWORD)-1, DWORD y = (DWORD)-1);

private:
	Config *m_lpConfig = nullptr;
	Input *m_lpInput = nullptr;
	Graphics *m_lpGraphics = nullptr;
	ResCache *m_lpCache = nullptr;
	VirtFs *m_lpVirtFs = nullptr;
	Level *m_lpLevel = nullptr;
	Walkthrough *m_lpWalkthrough = nullptr;

	BaseRunner * m_lpRunners[MAX_RUNNERS];
	Runner m_eCurrentRunner = NONE;
	BOOL m_bPaused = true;
};
