#include "engine.hh"
#include "game\player.hh"

Player::Player() {
	m_lpDrawObj = new DObject(
		Engine::GetInstance()->SysCache()->GetMesh("gfx\\weihnachtsman_000.x"),
		PLAYER_DEF_POSITION, PLAYER_DEF_ROTATION, 0.014f
	);

	m_lpDrawObj->f_lpMesh->SetBoundMin({-20.0f,  -0.8f, -20.0f});
	m_lpDrawObj->f_lpMesh->SetBoundMax({ 20.0f, 120.0f,  20.0f});
}

Player::~Player() {
	delete m_lpDrawObj;
}

void Player::Update(FLOAT delta) {
	auto engine = Engine::GetInstance();
	auto level = engine->SysLevel();
	struct PhyState {
		FLOAT ground = 0.0f;
		FLOAT friction = 0.0f;
		FLOAT *vely = nullptr;
	} ps;

	ps.vely = &m_vVelocity.y;
	BOOL touching =
	level->IterTouches(m_lpDrawObj, [](DObject *me, DObject *second, FLOAT floor, void *ud)->BOOL {
		auto elem = second->f_lpElem;
		auto ps = (PhyState *)ud;
		Level::ElevatorData *eldata;

		switch (elem->f_eType) {
			case Elems::JUMPER:
				*ps->vely = 17.5f;
				break;
			case Elems::ELEVATOR:
			case Elems::MOVER:
				eldata = (Level::ElevatorData *)second->f_lpUserData;
				if (eldata->f_dwState == 0) eldata->f_dwState = 1;
				/* Деление на 4 необходимо из-за четырёхкратной точности физики */
				me->f_vPos -= eldata->f_vMove / 4.0f;
				me->f_bAlerted = true;
				me->f_vPos.y = floor;
			/* fallthrough */

			case Elems::PLATFORM:
			case Elems::RECTFORM:
				ps->ground = floor;
				ps->friction = elem->f_fFriction;
				return true;
		}

		return false;
	}, &ps);

	level->IterTouches(m_lpDrawObj, [](DObject *, DObject *second, FLOAT, void *ud)->BOOL {
		switch(second->f_lpElem->f_eType) {
			case Elems::SAVEPOINT:
				((Player *)ud)->m_vSavePos = second->f_vPos;
				((Player *)ud)->m_vSaveRot.y = second->f_vRot.y + (D3DX_PI / 2.0f);
			case Elems::BONUS:
			case Elems::EXTRALIFE:
				second->f_bHidden = true;
				return true;
		}

		return false;
	}, this);

	if (touching && m_vVelocity.y <= 0.0f) {
		auto factor = 1.0f - (delta * (10.5f - ps.friction * 10.0f));
		m_vVelocity.x *= factor, m_vVelocity.z *= factor;
		if (!m_bIsTouchingGround) {
			m_dwJumpsLeft = 2;
			m_vVelocity.y = 0.0f;
			m_bIsTouchingGround = true;
			m_lpDrawObj->f_vPos.y = ps.ground;
		}
	} else if (!touching || m_vVelocity.y != 0.0f) {
		m_vVelocity.y -= delta * (2.0f * 9.81f);
		m_bIsTouchingGround = false;
	}

	m_lpDrawObj->f_vPos += m_vVelocity * delta;
	m_lpDrawObj->f_bAlerted = true;

	if (m_lpDrawObj->f_vPos.y < -50.0f) {
		m_lpDrawObj->f_vPos = m_vSavePos;
		m_lpDrawObj->f_vRot = m_vSaveRot;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
	}
}

void Player::Draw(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj->Draw(device);
}
