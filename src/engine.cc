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

	m_lpGraphics = new Graphics(hInst);
	m_lpInput = new Input(hInst, m_lpGraphics->GetWindow());
	m_lpVirtFs = new VirtFs("xmas.xpk");
	m_lpLevel = new Level();
	m_lpLevel->Load("levels\\008.dat");

	m_vRunners.push_back(new Editor());
	m_vRunners.push_back(new Game());

	m_lpGraphics->Show();
}

Engine::~Engine() {
	delete m_lpGraphics;
	delete m_lpInput;
	delete m_lpVirtFs;
	delete m_lpLevel;
	for (auto runner : m_vRunners)
		delete runner;
}

Engine *Engine::GetInstance() {
	return __engineInstance;
}

void Engine::SetRunner(DWORD num) {
	if (auto runner = GetRunner())
		runner->Close();
	
	m_dwCurrentRunner = num;

	if (auto runner = GetRunner())
		runner->Open();
}

BaseRunner *Engine::GetRunner() {
	if (m_dwCurrentRunner == m_vRunners.size()) return nullptr;
	return m_vRunners[m_dwCurrentRunner];
}

void Engine::Step(FLOAT delta) {
	if (auto runner = GetRunner()) {
		m_lpInput->Update(delta, runner);
		runner->OnUpdate(delta);

		if (auto device = m_lpGraphics->BeginFrame()) {
			m_lpLevel->Draw(device);
			runner->OnDraw(device);
			m_lpGraphics->EndFrame();
		}

		m_lpGraphics->PresentFrame();
	}
}
