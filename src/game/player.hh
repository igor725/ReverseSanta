#pragma once

#include "level.hh"
#include "dobject.hh"

#define PLAYER_DEF_POSITION {0.0f, 1.0f, 0.0f}
#define PLAYER_DEF_ROTATION {0.0f, -D3DX_PI / 2.0f, 0.0f}

class Player {
public:
	Player();
	~Player();

	BOOL Update(Level *level, FLOAT delta);
	void Draw(LPDIRECT3DDEVICE9 device);

	inline void Rotate(FLOAT dir) { m_lpDrawObj->f_vRot.y += dir; m_lpDrawObj->f_bAlerted = true; }
	inline void Jump() {
		if (m_dwJumpsLeft > 0) {
			m_vVelocity.y = 11.5f * (m_dwJumpsLeft / 2.0f);
			m_dwJumpsLeft -= m_bIsTouchingGround ? 1 : m_dwJumpsLeft;
			m_lpDrawObj->f_bAlerted = true;
		}
	}
	inline void SetXZVelocity(D3DXVECTOR3 diff) {
		m_vVelocity.x = diff.x, m_vVelocity.z = diff.z;
		m_lpDrawObj->f_bAlerted = true;
	}
	inline void SetSavePosition(LPD3DXVECTOR3 pos, FLOAT rot) {
		m_vSavePos = *pos, m_vSaveRot.y = rot;
	}
	inline void ResetPosition() {
		m_vSavePos = m_lpDrawObj->f_vPos = PLAYER_DEF_POSITION;
		m_vSaveRot = m_lpDrawObj->f_vRot = PLAYER_DEF_ROTATION;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
	}
	inline void Respawn() {
		m_lpDrawObj->f_vPos = m_vSavePos;
		m_lpDrawObj->f_vRot = m_vSaveRot;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
	}

	inline BOOL IsOnGround() { return m_bIsTouchingGround; }
	inline DObject *GetDrawObject() { return m_lpDrawObj; }
	inline D3DXVECTOR3 GetForward() {
		return {
			std::sinf(m_lpDrawObj->f_vRot.y) * std::cosf(m_lpDrawObj->f_vRot.x),
			std::sinf(-m_lpDrawObj->f_vRot.x),
			std::cosf(m_lpDrawObj->f_vRot.y) * std::cosf(m_lpDrawObj->f_vRot.x)
		};
	}

private:
	DObject *m_lpDrawObj = nullptr;

	DWORD m_dwJumpsLeft = 2;
	BOOL m_bIsTouchingGround = false;
	D3DXVECTOR3 
	m_vSavePos = {0.0f, 0.0f, 0.0f},
	m_vSaveRot = {0.0f, 0.0f, 0.0f},
	m_vVelocity = {0.0f, 0.0f, 0.0f};
};
