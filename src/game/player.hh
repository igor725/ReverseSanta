#pragma once

#include "draw.hh"

class Player {
public:
	Player(LPDIRECT3DDEVICE9 device);
	~Player();

	inline DObject *GetDrawObject() { return m_lpDrawObj; }

	void Update(FLOAT delta);
	void Draw(LPDIRECT3DDEVICE9 device);

private:
	DObject *m_lpDrawObj;
};
