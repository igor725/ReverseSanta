#include "game.hh"
#include "engine.hh"

Game::Game() {
	m_lpPlayer = new Player;
}

Game::~Game() {
	delete m_lpPlayer;
}

void Game::OnOpen() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto pobj = m_lpPlayer->GetDrawObject();
	camera->SetFollow(&pobj->f_vPos, &pobj->f_vRot);
	m_lpPlayer->ResetPosition();
}

void Game::OnClose() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	camera->SetFollow();
}

LRESULT Game::OnWndProc(HWND, UINT iMsg, WPARAM wParam, LPARAM) {
	auto engine = Engine::GetInstance();
	if (iMsg == WM_KEYUP) {
		if (wParam == VK_F1) {
			engine->SysInput()->Release();
			m_Menu.Toggle();
			return true;
		}
	}

	return false;
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
	auto engine = Engine::GetInstance();
	auto level = engine->SysLevel();

	struct TouchState {
		Level *level;
		Player *player;
		WalkthroughMan *wman;
	} ts;

	for (DWORD i = 0; i < 4; i++)
		m_lpPlayer->Update(level, delta / 4.0f);

	ts.wman = &m_Walkthrough;
	ts.player = m_lpPlayer;
	ts.level = level;

	level->IterTouches(m_lpPlayer->GetDrawObject(), [](DObject *, DObject *second, FLOAT, void *ud)->BOOL {
		auto *ts = (TouchState *)ud;

		switch(second->f_lpElem->f_eType) {
			case Elems::SAVEPOINT:
				ts->player->SetSavePosition(&second->f_vPos, second->f_vRot.y + (D3DX_PI / 2.0f));
			case Elems::BONUS:
			case Elems::EXTRALIFE:
				second->f_bHidden = true;
				return true;
			case Elems::EXIT:
				ts->wman->NextLevel(ts->level);
				ts->player->ResetPosition();
				return true;
		}

		return false;
	}, &ts);
}

void Game::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}

void Game::OnDrawUI() {
	m_Menu.Draw();
}
