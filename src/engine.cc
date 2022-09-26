#include "engine.hh"

#include "runners\editor.hh"
#include "runners\game.hh"
#include "runners\menu.hh"

static Engine *__engineInstance = nullptr;

Engine::Engine(HINSTANCE hInst) {
	__engineInstance = this;

	m_lpGraphics = new Graphics(hInst);
	m_lpConfig = new Config("config.dat", [](Config *cfg) {
		// Эта функция вызывается, когда обновляется конфиг
		auto engine = Engine::GetInstance();
		auto graphics = engine->SysGraphics();
		auto camera = graphics->GetCamera();

		if (cfg->IsChanged(Config::Camera)) {
			camera->f_fFov = cfg->GetFOV();
			camera->f_fRenderDistance = cfg->GetRenderDistance();
			camera->UpdateProj();
		}

		if (cfg->IsChanged(Config::Graphics)) {
			graphics->UpdateWindow(
				cfg->GetBorderless(),
				cfg->GetWidth(),
				cfg->GetHeight()
			);
		}
	});
	m_lpCache = new ResCache;
	m_lpVirtFs = new VirtFs(m_lpConfig->GetArchivePath());
	m_lpInput = new Input(hInst, m_lpGraphics->GetWindow());
	m_lpLevel = new Level;
	m_lpWalkthrough = new Walkthrough(m_lpLevel);

	m_lpRunners[MENU] = new Menu;
	m_lpRunners[EDITOR] = new Editor;
	m_lpRunners[GAME] = new Game;

	m_lpGraphics->Show();
	SetRunner(MENU);
}

Engine::~Engine() {
	delete m_lpGraphics;
	delete m_lpConfig;
	delete m_lpInput;
	delete m_lpVirtFs;
	delete m_lpCache;
	delete m_lpLevel;
	for (DWORD i = MENU; i < MAX_RUNNERS; i++)
		delete m_lpRunners[i];
	__engineInstance = nullptr;
}

Engine *Engine::GetInstance() {
	return __engineInstance;
}

VOID Engine::OnDeviceLost() {
	m_lpCache->OnDeviceLost();
	for (DWORD i = MENU; i < MAX_RUNNERS; i++)
		m_lpRunners[i]->OnDeviceLost();
}

VOID Engine::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	m_lpCache->OnDeviceReset(device);
	for (DWORD i = MENU; i < MAX_RUNNERS; i++)
		m_lpRunners[i]->OnDeviceReset(device);
}

VOID Engine::SetRunner(Runner num) {
	if (m_eCurrentRunner == num) return;

	if (auto runner = GetRunner())
		runner->OnClose();

	auto prev = m_eCurrentRunner;
	m_eCurrentRunner = num;

	if (auto runner = GetRunner())
		runner->OnOpen(prev);
}

VOID Engine::SetPause(BOOL state) {
	if (m_bPaused == state) return;
	m_bPaused = state;
	if (auto runner = GetRunner())
		runner->OnPause(state);
	if (state) m_lpInput->Release();
	else m_lpInput->Capture();
}

VOID Engine::Step(FLOAT delta) {
	delta = min(delta, 0.016f);
	if (auto runner = GetRunner()) {
		if (!m_bPaused || m_eCurrentRunner == MENU) {
			m_lpInput->Update(delta, runner);
			for (DWORD i = 0; i < 4; i++) {
				const auto hdelta = delta / 4.0f;
				m_lpLevel->Update(hdelta);
				runner->OnUpdate(hdelta);
			}
		}

		if (m_lpGraphics->TestDevice()) {
			if (auto device = m_lpGraphics->BeginFrame(delta)) {
				m_lpLevel->Draw(device, m_lpGraphics->GetCamera());
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
		m_lpLevel->Draw(device, nullptr, true);
		m_lpGraphics->EndFrame();
		auto sur = m_lpGraphics->PresentToSurface();
		D3DLOCKED_RECT lock; D3DSURFACE_DESC desc;

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
