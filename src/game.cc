#include "game.hh"
#include "engine.hh"

Game::Game() {
	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();

	m_lpPlayer = new Player(graphics->GetDevice());
}

Game::~Game() {
	delete m_lpPlayer;
}

void Game::OnDeviceLost() {
	m_lpPlayer->OnDeviceLost();
}

void Game::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->OnDeviceReset(device);
}

void Game::OnOpen() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto pobj = m_lpPlayer->GetDrawObject();
	camera->SetFollow(&pobj->f_vPos, &pobj->f_vRot);
}

void Game::OnClose() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	camera->SetFollow();
}

void Game::OnInput(FLOAT delta, InputState *state) {
	if (state->Key(DIK_A) & 0x80)
		m_lpPlayer->Rotate((D3DX_PI * -1.1f) * delta);
	else if (state->Key(DIK_D) & 0x80)
		m_lpPlayer->Rotate((D3DX_PI * 1.1f) * delta);

	if (state->Key(DIK_W) & 0x80)
		m_lpPlayer->SetXZVelocity(m_lpPlayer->GetForward() * -6.0f);
	else if (state->Key(DIK_S) & 0x80)
		m_lpPlayer->SetXZVelocity(m_lpPlayer->GetForward() * 6.0f);

	static BYTE lsp, csp;
	if ((csp = state->Key(DIK_SPACE)) != lsp) {
		if (csp & 0x80) m_lpPlayer->Jump();
		lsp = csp;
	}
}

void Game::OnUpdate(FLOAT delta) {
	for (FLOAT i = 0.0f; i < 6.0f; i++)
		m_lpPlayer->Update(delta / 6.0f);
}

void Game::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}
