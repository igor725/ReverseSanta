#include "game.hh"
#include "engine.hh"

Game::Game() {
	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();

	m_lpPlayer = new Player(graphics->GetDevice());
}

Game::~Game() {}

void Game::OnUpdate(FLOAT delta) {
	auto engine = Engine::GetInstance();
	auto graphics = engine->SysGraphics();
	auto camera = graphics->GetCamera();

	m_lpPlayer->Update(delta);
	camera->Follow(delta, m_lpPlayer->GetDrawObject());
}

void Game::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}
