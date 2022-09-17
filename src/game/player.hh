#pragma once

#include "dobject.hh"

class Player {
public:
	Player(LPDIRECT3DDEVICE9 device) { OnDeviceReset(device); };
	~Player();

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	void Update(FLOAT delta);
	void Draw(LPDIRECT3DDEVICE9 device);

	inline void Rotate(FLOAT dir) { m_lpDrawObj->f_vRot.y += dir; m_lpDrawObj->f_bAlerted = true; }
	inline void Move(D3DXVECTOR3 diff) { m_lpDrawObj->f_vPos += diff; m_lpDrawObj->f_bAlerted = true; }
	inline void Jump() {
		if (m_dwJumpsLeft > 0) {
			if (m_bIsTouchingGround)
				m_dwJumpsLeft -= 1;
			else
				m_dwJumpsLeft = 0;
			m_vVelocity.y = 14.0f;
			m_lpDrawObj->f_bAlerted = true;
		}
	}
	inline void SetXZVelocity(D3DXVECTOR3 diff) {
		m_vVelocity.x = diff.x;
		m_vVelocity.z = diff.z;
		m_lpDrawObj->f_bAlerted = true;
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
	D3DXVECTOR3 m_vSavePos = {0.0f, 0.0f, 0.0f},
	m_vSaveRot = {0.0f, 0.0f, 0.0f},
	m_vVelocity = {0.0f, 0.0f, 0.0f};
};
