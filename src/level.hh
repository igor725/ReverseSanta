#pragma once

#include <Windows.h>
#include <vector>
#include <iterator>

#include "elems.hh"
#include "draw.hh"

class Level {
public:
	struct Object {
		CHAR f_name[32];
		D3DXVECTOR3 f_pos[2];
		DWORD f_rot;
	};

	Level();
	~Level();

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	bool Load(std::string path);
	void RefreshDrawer();
	void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false);

private:
	Elems *m_lpElems = nullptr;
	DWORD m_dwObjectCount = 0;
	Object *m_lpObjects = nullptr;
	DObject *m_lpDObjects = nullptr;
	DObject *m_lpSkyBox = nullptr;
	LPDIRECT3DTEXTURE9 m_lpTempTexture = nullptr;
	LPDIRECT3DSURFACE9 m_lpTempSurface = nullptr;
};
