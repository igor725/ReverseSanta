#pragma once

#include "draw.hh"

class Player {
public:
	Player(LPDIRECT3DDEVICE9 device);
	~Player();

	inline DObject *GetDrawObject() { return m_lpDrawObj; }

	void Update(FLOAT delta);
	void Draw(LPDIRECT3DDEVICE9 device);

	inline void Rotate(FLOAT dir) { m_lpDrawObj->f_rot.y += dir; m_lpDrawObj->f_alerted = true; }
	inline void Move(D3DXVECTOR3 diff) { m_lpDrawObj->f_pos += diff; m_lpDrawObj->f_alerted = true; }

	inline D3DXVECTOR3 Forward() {
		return {
			std::sinf(m_lpDrawObj->f_rot.y) * std::cosf(m_lpDrawObj->f_rot.x),
			std::sinf(-m_lpDrawObj->f_rot.x),
			std::cosf(m_lpDrawObj->f_rot.y) * std::cosf(m_lpDrawObj->f_rot.x)
		};
	}

private:
	DObject *m_lpDrawObj;
};
