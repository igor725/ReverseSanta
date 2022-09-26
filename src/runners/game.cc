#include "game.hh"
#include "engine.hh"

VOID Game::OnOpen(DWORD prev) {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	auto walk = engine->SysWalkthrough();
	auto &wcfg = walk->GetConfig();
	auto pobj = m_lpPlayer->GetDrawObject();

	camera->SetFollow(&pobj->f_vPos, &pobj->f_vRot);
	if (prev == Engine::EDITOR)
		walk->Begin();
	else {
		engine->SetPause(false);
		walk->Reset();
	}
	m_lpPlayer->SetAirJumps(wcfg.f_bTripleJump ? 3 : 2);
}

VOID Game::OnClose() {
	auto engine = Engine::GetInstance();
	auto camera = engine->SysGraphics()->GetCamera();
	m_lpPlayer->ResetPosition();
	camera->SetFollow();
}

VOID Game::OnPause(BOOL state) {
	m_Menu.SetState(state);
}

VOID Game::OnInput(FLOAT delta, InputState *state) {
	if (state->CurMoved())
		m_lpPlayer->Rotate(state->CurCX() * 0.001f);

	if (state->KBIsDown(DIK_A))
		m_lpPlayer->Rotate((D3DX_PI * -1.1f) * delta);
	else if (state->KBIsDown(DIK_D))
		m_lpPlayer->Rotate((D3DX_PI * 1.1f) * delta);

	if (state->KBIsDown(DIK_W) || state->MBIsDown(1))
		m_lpPlayer->SetXZVelocity(m_lpPlayer->GetForward() * -6.0f);
	else if (state->KBIsDown(DIK_S))
		m_lpPlayer->SetXZVelocity(m_lpPlayer->GetForward() * 6.0f);

	if (state->KBIsJustDown(DIK_SPACE) || state->MBIsJustDown(0))
		m_lpPlayer->Jump();
}

VOID Game::OnUpdate(FLOAT delta) {
	auto engine = Engine::GetInstance();
	auto level = engine->SysLevel();
	auto walk = engine->SysWalkthrough();

	struct TouchState {
		Level *level;
		Player *player;
		Walkthrough *wth;
	} ts;

	if (!m_lpPlayer->Update(level, delta)) {
		m_lpPlayer->Respawn();
		EASSERT(walk->Death() && "No more lives left, game over!");
	}

	ts.wth = walk;
	ts.player = m_lpPlayer;
	ts.level = level;

	level->IterTouches(m_lpPlayer->GetDrawObject(), [](DObject *, DObject *second, FLOAT, LPVOID ud)->BOOL {
		auto *ts = (TouchState *)ud;

		switch(second->f_lpElem->f_eType) {
			case Elems::BONUS:
				ts->wth->CollectBonus();
				second->f_bHidden = true;
				return true;
			case Elems::SAVEPOINT:
				if (!ts->wth->SavePointUsed()) break;
				ts->player->SetSavePosition(&second->f_vPos, second->f_vRot.y + (D3DX_PI * 0.5f));
				second->f_bHidden = true;
				return true;
			case Elems::EXTRALIFE:
				if (!ts->wth->SavePointUsed()) break;
				ts->wth->CollectLive();
				second->f_bHidden = true;
				return true;
			case Elems::EXIT:
				if (ts->wth->NextLevel())
					ts->player->ResetPosition();
				else
					ts->player->Recover();
				return true;
		}

		return false;
	}, &ts);

	EASSERT(walk->Update(delta) && "Time's up, game over!");
}

VOID Game::OnDraw(LPDIRECT3DDEVICE9 device) {
	m_lpPlayer->Draw(device);
}

VOID Game::OnDrawUI() {
	m_Menu.Draw();
}
