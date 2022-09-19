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

	struct ElevatorData {
		BOOL f_bIsActive = false,
		f_bStopOnFinish = false;
		FLOAT f_fTime = 3.0f / 4.0f;

		ElevatorData(Elems::Type type) : f_bIsActive(type == Elems::ELEVATOR) {}

		void Update(DObject *dobj, LObject *lobj, FLOAT delta) {
			if (!f_bIsActive) return;
			dobj->f_bAlerted = true;
			auto dir = lobj->f_vPos[1] - lobj->f_vPos[0];
			auto mult = (0.5f * (1.0f + std::sinf(2 * D3DX_PI * f_fTime)));
			dobj->f_vPos = lobj->f_vPos[0] + dir * mult;
			auto len = D3DXVec3Length(&dir);
			f_fTime += delta / (len / 2.0f);
		}
	};

	Level();
	~Level();

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	void Rebuild();
	BOOL Load(std::string path);
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
