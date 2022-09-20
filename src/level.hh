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
		DWORD f_dwState = 0,
		f_bStopOnFinish = false;
		FLOAT f_fTime = 0.75f, f_fMult = 0.0f;
		D3DXVECTOR3 f_vMove = {0.0f, 0.0f, 0.0f};

		ElevatorData(Elems::Type type) : f_dwState(type == Elems::ELEVATOR), f_bStopOnFinish(f_dwState == 0) {}

		void Update(DObject *dobj, LObject *lobj, FLOAT delta) {
			if (f_dwState < 1) return;
			dobj->f_bAlerted = true;
			auto prmult = f_fMult;
			f_fMult = (0.5f * (1.0f + std::sinf(2 * D3DX_PI * f_fTime)));
			if (f_bStopOnFinish) {
				if (f_dwState % 2 ? f_fMult < prmult : f_fMult > prmult) f_dwState++;
				if (f_dwState == 3) {
					f_vMove = {0.0f, 0.0f, 0.0f};
					f_fTime = 0.75f;
					f_dwState = 0;
					f_fMult = 0;
					return;
				}
			}
			f_vMove = dobj->f_vPos;
			auto dir = lobj->f_vPos[1] - lobj->f_vPos[0];
			dobj->f_vPos = lobj->f_vPos[0] + dir * f_fMult;
			f_vMove -= dobj->f_vPos;
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
	BOOL IterObjects(BOOL(*callback)(ObjectData data, void *ud), void *ud);
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
