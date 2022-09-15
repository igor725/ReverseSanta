#include "game\player.hh"

Player::Player(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj = new DObject(
		new Mesh(device, "gfx\\weihnachtsman_000.x"),
		{0.0f, 0.0f, 0.0f}, {0.0f, -D3DX_PI / 2.0f, 0.0f}, 0.014f
	);
}

Player::~Player() {
	delete m_lpDrawObj;
}

void Player::Update(FLOAT) {

}

void Player::Draw(LPDIRECT3DDEVICE9 device) {
	m_lpDrawObj->Draw(device);
}
