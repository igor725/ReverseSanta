#pragma once

#include "level.hh"
#include "dobject.hh"

#define PLAYER_DEF_POSITION {0.0f, 1.0f, 0.0f}
#define PLAYER_DEF_ROTATION {0.0f, -D3DX_PI * 0.5f, 0.0f}

class Player {
public:
	Player();
	~Player() { delete m_lpDrawObj; }

	BOOL Update(Level *level, FLOAT delta);
	VOID Draw(LPDIRECT3DDEVICE9 device);

	inline VOID Rotate(FLOAT dir) { m_lpDrawObj->f_vRot.y += dir; }
	inline VOID Jump() {
		if (m_dwJumpsLeft > 0) {
			m_vVelocity.y = max(m_vVelocity.y, 11.5f * m_dwJumpsLeft / (FLOAT)m_dwJumpsCount);
			m_dwJumpsLeft -= m_dwJumpsCount > 2 ? 1 : m_bIsTouchingGround ? 1 : m_dwJumpsLeft;
		}
	}
	inline VOID SetXZVelocity(D3DXVECTOR3 diff) {
		m_vVelocity.x = diff.x, m_vVelocity.z = diff.z;
	}
	inline VOID SetAirJumps(DWORD count) { m_dwJumpsCount = count; }
	inline VOID SetSavePosition(const LPD3DXVECTOR3 pos, FLOAT rot) {
		m_vSavePos = *pos, m_vSaveRot.y = rot;
	}
	inline VOID ResetPosition() {
		m_vSavePos = m_lpDrawObj->f_vPos = PLAYER_DEF_POSITION;
		m_vSaveRot = m_lpDrawObj->f_vRot = PLAYER_DEF_ROTATION;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
		m_lpLastStand = nullptr;
	}
	inline VOID Recover() {
		if (!m_lpLastStand) {
			ResetPosition();
			return;
		}
		m_lpDrawObj->f_vPos = m_lpLastStand->f_vPos;
	}
	inline VOID Respawn() {
		m_lpDrawObj->f_vPos = m_vSavePos;
		m_lpDrawObj->f_vRot = m_vSaveRot;
		m_vVelocity = {0.0f, 0.0f, 0.0f};
		m_lpLastStand = nullptr;
	}

	inline DObject *GetDrawObject() { return m_lpDrawObj; }
	inline D3DXVECTOR3 GetForward() {
		return {
			std::sinf(m_lpDrawObj->f_vRot.y) * std::cosf(m_lpDrawObj->f_vRot.x),
			std::sinf(-m_lpDrawObj->f_vRot.x),
			std::cosf(m_lpDrawObj->f_vRot.y) * std::cosf(m_lpDrawObj->f_vRot.x)
		};
	}

private:
	DObject *m_lpDrawObj = nullptr,
	*m_lpLastStand = nullptr;

	DWORD m_dwJumpsLeft = 2,
	m_dwJumpsCount = 2;
	BOOL m_bIsTouchingGround = false;
	D3DXVECTOR3 
	m_vSavePos = {0.0f, 0.0f, 0.0f},
	m_vSaveRot = {0.0f, 0.0f, 0.0f},
	m_vVelocity = {0.0f, 0.0f, 0.0f};
};
