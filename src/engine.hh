#pragma once

#include <Windows.h>
#include <vector>

#include "graphics.hh"
#include "input.hh"
#include "virtfs.hh"
#include "level.hh"
#include "base_runner.hh"

class Engine {
public:
	static Engine *GetInstance();
	Engine(HINSTANCE hInst);
	~Engine();

	BaseRunner *GetRunner();
	void SetRunner(DWORD num);
	void NextRunner();

	void Step(FLOAT delta);

	inline Input *Engine::SysInput() { return m_lpInput; }
	inline Graphics *Engine::SysGraphics() { return m_lpGraphics; }
	inline Level *Engine::SysLevel() { return m_lpLevel; }
	inline VirtFs *Engine::SysVirtFs() { return m_lpVirtFs; }

private:
	Input *m_lpInput = nullptr;
	Graphics *m_lpGraphics = nullptr;
	VirtFs *m_lpVirtFs = nullptr;
	Level *m_lpLevel = nullptr;

	std::vector<BaseRunner *> m_vRunners;
	DWORD m_dwCurrentRunner = 0;
};
