#include "engine.hh"

#include "graphics.hh"
#include "input.hh"
#include "virtfs.hh"
#include "level.hh"
#include "editor.hh"
#include "game.hh"

static Engine *__engineInstance = nullptr;

Engine::Engine(HINSTANCE hInst) {
	__engineInstance = this;

	m_lpCache = new ResCache;
	m_lpVirtFs = new VirtFs("xmas.xpk");
	m_lpGraphics = new Graphics(hInst);
	m_lpInput = new Input(hInst, m_lpGraphics->GetWindow());
	m_lpLevel = new Level;
	EASSERT(m_lpLevel->Load("levels\\000.dat"));

	m_vRunners.push_back(new Editor);
	m_vRunners.push_back(new Game);

	m_lpGraphics->Show();
}

Engine::~Engine() {
	delete m_lpGraphics;
	delete m_lpInput;
	delete m_lpVirtFs;
	delete m_lpCache;
	delete m_lpLevel;
	for (auto runner : m_vRunners)
		delete runner;
	__engineInstance = nullptr;
}

Engine *Engine::GetInstance() {
	return __engineInstance;
}

void Engine::OnDeviceLost() {
	m_lpCache->OnDeviceLost();
	for (auto &runner : GetRunners())
		runner->OnDeviceLost();
}

void Engine::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	m_lpCache->OnDeviceReset(device);
	for (auto &runner : GetRunners())
		runner->OnDeviceReset(device);
}

void Engine::SetRunner(DWORD num) {
	if (auto runner = GetRunner())
		runner->OnClose();

	m_dwCurrentRunner = num;

	if (auto runner = GetRunner())
		runner->OnOpen();
}

void Engine::NextRunner() {
	DWORD nextrunner = (m_dwCurrentRunner + 1);
	if ((m_dwCurrentRunner + 1) == m_vRunners.size())
		nextrunner = 0;
	SetRunner(nextrunner);
}

BaseRunner *Engine::GetRunner() {
	if (m_dwCurrentRunner == m_vRunners.size()) return nullptr;
	return m_vRunners[m_dwCurrentRunner];
}

void Engine::Step(FLOAT delta) {
	delta = min(delta, 0.016f);
	if (auto runner = GetRunner()) {
		m_lpInput->Update(delta, runner);
		for (DWORD i = 0; i < 4; i++) {
			m_lpLevel->Update(delta / 4.0f);
			runner->OnUpdate(delta / 4.0f);
		}

		if (m_lpGraphics->TestDevice()) {
			if (auto device = m_lpGraphics->BeginFrame(delta)) {
				m_lpLevel->Draw(device);
				runner->OnDraw(device);

				m_lpGraphics->StartUI();
				runner->OnDrawUI();
				m_lpGraphics->EndUI();

				m_lpGraphics->EndFrame();
				m_lpGraphics->PresentFrame();
			}
		}
	}
}

BOOL Engine::GetObjectOn(Level::ObjectData *data, DWORD x, DWORD y) {
	if (auto device = m_lpGraphics->BeginFrame(0)) {
		m_lpLevel->Draw(device, true);
		m_lpGraphics->EndFrame();
		auto sur = m_lpGraphics->PresentToSurface();
		D3DLOCKED_RECT lock;
		D3DSURFACE_DESC desc;
		if (sur->GetDesc(&desc) == D3D_OK) {
			if (x == -1 && y == -1)
				x = desc.Width / 2, y = desc.Height / 2;
			else if (x >= desc.Width || y >= desc.Height)
				return false;
		} else
			return false;

		if (sur->LockRect(&lock, nullptr, D3DLOCK_READONLY) == D3D_OK) {
			auto bits = (BYTE *)lock.pBits;
			auto row = (DWORD *)(bits + (DWORD)y * lock.Pitch);
			auto ret = m_lpLevel->GetObjectData(row[x], data);
			sur->UnlockRect();
			return ret;
		}
	}

	return false;
}
