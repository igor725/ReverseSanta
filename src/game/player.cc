#include "engine.hh"
#include "game\player.hh"

#define PLAYER_DEF_POSITION {0.0f, 1.0f, 0.0f}
#define PLAYER_DEF_ROTATION {0.0f, -D3DX_PI / 2.0f, 0.0f}

Player::~Player() {
	delete m_lpDrawObj;
}

void Player::OnDeviceLost() {
	Player::~Player();
}

void Player::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj = new DObject(
		new Mesh(device, "gfx\\weihnachtsman_000.x"),
		PLAYER_DEF_POSITION, PLAYER_DEF_ROTATION, 0.014f
	);

	m_lpDrawObj->f_lpMesh->SetBoundMin({-20.0f, -0.8f, -20.0f});
	m_lpDrawObj->f_lpMesh->SetBoundMax({ 20.0f, 15.0f,  20.0f});
}

void Player::Update(FLOAT delta) {
	auto engine = Engine::GetInstance();
	auto level = engine->SysLevel();
	FLOAT ground = 0.0f;
	BOOL touching = level->IsTouching(m_lpDrawObj, &ground);

	if (touching && m_vVelocity.y <= 0.0f) {
		auto factor = 1.0f - (delta * 6.0f);
		m_vVelocity.x = m_vVelocity.x * factor;
		m_vVelocity.z = m_vVelocity.z * factor;
		if (!m_bIsTouchingGround) {
			m_dwJumpsLeft = 2;
			m_vVelocity.y = 0.0f;
			m_bIsTouchingGround = true;
			m_lpDrawObj->f_vPos.y = ground;
		}
	} else if (!touching || m_vVelocity.y != 0.0f) {
		m_vVelocity.y -= delta * (5.0f * 9.81f);
		m_bIsTouchingGround = false;
	}

	Move(m_vVelocity * delta);

	if (m_lpDrawObj->f_vPos.y < -50.0f) {
		m_lpDrawObj->f_vPos = PLAYER_DEF_POSITION;
		m_lpDrawObj->f_vRot = PLAYER_DEF_ROTATION;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
	}
}

void Player::Draw(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj->Draw(device);
}
