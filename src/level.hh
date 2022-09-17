#pragma once

#include <Windows.h>
#include <vector>
#include <iterator>

#include "elems.hh"
#include "dobject.hh"

class Level {
public:
	struct LObject {
		CHAR f_name[32];
		D3DXVECTOR3 f_vPos[2];
		DWORD f_dwRot;
	};

	struct ObjectData {
		DObject *f_lpDObj;
		LObject *f_lpLObj;
	};

	Level();
	~Level();

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	BOOL Load(std::string path);
	void Refresh();
	void Update(FLOAT delta);
	void Draw(LPDIRECT3DDEVICE9 device, BOOL untextured = false);

	BOOL IterTouches(DObject *obj, BOOL(*callback)(DObject *first, DObject *other, FLOAT floor, void *ud), void *ud);
	BOOL GetObjectData(DWORD id, ObjectData *data);

private:
	Elems *m_lpElems = nullptr;
	DWORD m_dwObjectCount = 0;
	LObject *m_lpLObjects = nullptr;
	DObject *m_lpDObjects = nullptr;
	DObject *m_lpSkyBox = nullptr;
	LPDIRECT3DTEXTURE9 m_lpTempTexture = nullptr;
	LPDIRECT3DSURFACE9 m_lpTempSurface = nullptr;
};
