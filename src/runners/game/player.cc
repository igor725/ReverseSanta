#include "engine.hh"
#include "player.hh"

Player::Player() {
	m_lpDrawObj = new DObject(
		Engine::GetInstance()->SysCache()->GetMesh("gfx\\weihnachtsman_000.x"),
		PLAYER_DEF_POSITION, PLAYER_DEF_ROTATION, 0.014f
	);

	m_lpDrawObj->f_lpMesh->SetBoundMin({-20.0f,  -1.8f, -20.0f});
	m_lpDrawObj->f_lpMesh->SetBoundMax({ 20.0f, 120.0f,  20.0f});
}

BOOL Player::Update(Level *level, FLOAT delta) {
	struct PhyState {
		FLOAT ground = 0.0f;
		FLOAT friction = 0.0f;
		FLOAT *vely = nullptr;
		LPDWORD jumps = nullptr;
		DObject **stand = nullptr;
	} ps;

	ps.stand = &m_lpLastStand;
	ps.jumps = &m_dwJumpsLeft;
	ps.vely = &m_vVelocity.y;
	BOOL touching =
	level->IterTouches(m_lpDrawObj, [](DObject *me, DObject *second, FLOAT floor, LPVOID ud)->BOOL {
		auto elem = second->f_lpElem;
		auto ps = (PhyState *)ud;
		Level::ElevatorData *eldata;

		switch (elem->f_eType) {
			case Elems::JUMPER:
				*ps->vely = 17.5f;
				*ps->jumps = 1;
				break;
			case Elems::ELEVATOR:
			case Elems::MOVER:
				eldata = (Level::ElevatorData *)second->f_lpUserData;
				if (eldata->f_dwState == 0) eldata->f_dwState = 1;
				me->f_vPos -= eldata->f_vMove;
				me->f_bAlerted = true;
			/* fallthrough */
			case Elems::PLATFORM:
			case Elems::RECTFORM:
				ps->ground = floor;
				*ps->stand = second;
				ps->friction = elem->f_fFriction;
				return true;
		}

		return false;
	}, &ps);

	if (touching && m_vVelocity.y <= 0.0f) {
		auto factor = 1.0f - (delta * (10.5f - ps.friction * 10.0f));
		m_vVelocity.x *= factor, m_vVelocity.z *= factor;
		m_lpDrawObj->f_vPos.y = ps.ground;
		if (!m_bIsTouchingGround) {
 			m_dwJumpsLeft = 2;
			m_vVelocity.y = 0.0f;
			m_bIsTouchingGround = true;
		}
	} else if (!touching || m_vVelocity.y != 0.0f) {
		m_vVelocity.y -= delta * (2.0f * 9.81f);
		m_bIsTouchingGround = false;
	}

	m_lpDrawObj->f_vPos += m_vVelocity * delta;
	m_lpDrawObj->f_bAlerted = true;

	return m_lpDrawObj->f_vPos.y > -50.0f;
}

VOID Player::Draw(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj->Draw(device);
}
