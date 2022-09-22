#pragma once

#include <Windows.h>
#include <vector>

#include "graphics.hh"
#include "input.hh"
#include "virtfs.hh"
#include "level.hh"
#include "baserunner.hh"
#include "rescache.hh"

class Engine {
public:
	static Engine *GetInstance();

	Engine(HINSTANCE hInst);
	~Engine();

	VOID OnDeviceLost();
	VOID OnDeviceReset(LPDIRECT3DDEVICE9 device);

	BaseRunner *GetRunner();
	inline std::vector<BaseRunner *> &GetRunners() { return m_vRunners; }
	VOID SetRunner(DWORD num);
	VOID NextRunner();

	VOID Step(FLOAT delta);

	inline Input *SysInput() { return m_lpInput; }
	inline Graphics *SysGraphics() { return m_lpGraphics; }
	inline Level *SysLevel() { return m_lpLevel; }
	inline ResCache *SysCache() { return m_lpCache; }
	inline VirtFs *SysVirtFs() { return m_lpVirtFs; }

	BOOL GetObjectOn(Level::ObjectData *data, DWORD x = (DWORD)-1, DWORD y = (DWORD)-1);

private:
	Input *m_lpInput = nullptr;
	Graphics *m_lpGraphics = nullptr;
	ResCache *m_lpCache = nullptr;
	VirtFs *m_lpVirtFs = nullptr;
	Level *m_lpLevel = nullptr;

	std::vector<BaseRunner *> m_vRunners;
	DWORD m_dwCurrentRunner = 0;
};
