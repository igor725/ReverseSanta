#include "engine.hh"
#include "game\player.hh"

#define PLAYER_DEF_POSITION {0.0f, 1.0f, 0.0f}
#define PLAYER_DEF_ROTATION {0.0f, -D3DX_PI / 2.0f, 0.0f}

Player::~Player() {
	delete m_lpDrawObj;
}

void Player::OnDeviceLost() {
	m_lpDrawObj->f_lpMesh->OnDeviceLost();
}

void Player::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	if (!m_lpDrawObj) {
		m_lpDrawObj = new DObject(
			new Mesh(device, "gfx\\weihnachtsman_000.x"),
			PLAYER_DEF_POSITION, PLAYER_DEF_ROTATION, 0.014f
		);

		m_vSavePos = PLAYER_DEF_POSITION;
		m_vSaveRot = PLAYER_DEF_ROTATION;
		m_lpDrawObj->f_lpMesh->SetBoundMin({-20.0f,  -0.8f, -20.0f});
		m_lpDrawObj->f_lpMesh->SetBoundMax({ 20.0f, 120.0f,  20.0f});
	} else
		m_lpDrawObj->f_lpMesh->OnDeviceReset(device);
}

void Player::Update(FLOAT delta) {
	auto engine = Engine::GetInstance();
	auto level = engine->SysLevel();
	struct PhyState {
		FLOAT ground = 0.0f;
		FLOAT friction = 0.0f;
	} ps;

	BOOL touching =
	level->IterTouches(m_lpDrawObj, [](DObject *, DObject *second, FLOAT floor, void *ud)->BOOL {
		auto elem = second->GetElemInfo();
		auto ps = (PhyState *)ud;

		switch(elem->f_eType) {
			case Elems::PLATFORM:
			case Elems::RECTFORM:
			case Elems::ELEVATOR:
			case Elems::MOVER:
			case Elems::JUMPER:
				ps->ground = floor;
				ps->friction = elem->f_fFriction;
				return true;
		}

		return false;
	}, &ps);

	level->IterTouches(m_lpDrawObj, [](DObject *, DObject *second, FLOAT, void *ud)->BOOL {
		switch(second->GetElemInfo()->f_eType) {
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
		auto factor = 1.0f - (delta * (7.5f - ps.friction * 7.0f));
		m_vVelocity.x = m_vVelocity.x * factor;
		m_vVelocity.z = m_vVelocity.z * factor;
		if (!m_bIsTouchingGround) {
			m_dwJumpsLeft = 2;
			m_vVelocity.y = 0.0f;
			m_bIsTouchingGround = true;
			m_lpDrawObj->f_vPos.y = ps.ground;
		}
	} else if (!touching || m_vVelocity.y != 0.0f) {
		m_vVelocity.y -= delta * (5.0f * 9.81f);
		m_bIsTouchingGround = false;
	}

	Move(m_vVelocity * delta);

	if (m_lpDrawObj->f_vPos.y < -50.0f) {
		m_lpDrawObj->f_vPos = m_vSavePos;
		m_lpDrawObj->f_vRot = m_vSaveRot;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
	}
}

void Player::Draw(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj->Draw(device);
}
