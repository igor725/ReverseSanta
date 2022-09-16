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
	camera->SetFollow(&pobj->f_pos, &pobj->f_rot);
}

void Game::OnClose() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	camera->SetFollow(nullptr, nullptr);
}

void Game::OnInput(FLOAT delta, InputState *state) {
	if (state->Key(DIK_A) & 0x80)
		m_lpPlayer->Rotate(delta * (D3DX_PI * -1.2f));
	else if (state->Key(DIK_D) & 0x80)
		m_lpPlayer->Rotate(delta * (D3DX_PI * 1.2f));

	if (state->Key(DIK_W) & 0x80)
		m_lpPlayer->Move(m_lpPlayer->GetForward() * delta * -10.0f);
	else if (state->Key(DIK_S) & 0x80)
		m_lpPlayer->Move(m_lpPlayer->GetForward() * delta * 10.0f);
}

void Game::OnUpdate(FLOAT delta) {
	// auto engine = Engine::GetInstance();
	// auto graphics = engine->SysGraphics();
	// auto camera = graphics->GetCamera();

	m_lpPlayer->Update(delta);
}

void Game::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}
