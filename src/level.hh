#pragma once

#include <Windows.h>
#include <vector>
#include <iterator>

#include "elems.hh"
#include "dobject.hh"
#include "exceptions.hh"

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

		VOID Update(DObject *dobj, LObject *lobj, FLOAT delta);
	};

	struct EnemyData {
		BOOL f_bEnabled = true;
		DWORD f_dwBehaviourHash;
		DWORD f_dwState = 0;
		FLOAT f_fFwdX = 0.0f, f_fFwdZ = 0.0f;

		EnemyData(DWORD hash) : f_dwBehaviourHash(hash) {}

		BOOL CheckForward(Level *level, DObject *me);

		VOID CrowTick(Level *, DObject *, FLOAT) {}
		VOID SnowmanTick(Level *, DObject *, FLOAT) {}
		VOID TrollTick(Level *level, DObject *me, FLOAT delta);

		VOID Update(Level *level, DObject *self, FLOAT delta);
	};

	Level() : m_Elems("data\\elements.txt") {}
	~Level() { Cleanup(); }

	VOID OnDeviceLost();
	VOID OnDeviceReset(LPDIRECT3DDEVICE9 device);

	VOID Cleanup();
	VOID Rebuild();
	BOOL Load(std::string path);
	VOID Update(FLOAT delta);
	VOID Draw(LPDIRECT3DDEVICE9 device, Camera *camera = nullptr, BOOL untextured = false);

	BOOL IterTouches(DObject *obj, BOOL(*callback)(DObject *first, DObject *other, FLOAT floor, LPVOID ud), LPVOID ud = nullptr);
	BOOL IsTouchingAnything(LPD3DXVECTOR3 point, DObject *except);
	BOOL IterObjects(BOOL(*callback)(ObjectData data, LPVOID ud), LPVOID ud = nullptr);
	BOOL GetObjectData(DWORD id, ObjectData *data);

private:
	Elems m_Elems;
	DWORD m_dwObjectCount = 0;
	LObject *m_lpLObjects = nullptr;
	DObject *m_lpDObjects = nullptr;
	DObject *m_lpSkyBox = nullptr;
	LPDIRECT3DTEXTURE9 m_lpTempTexture = nullptr;
	LPDIRECT3DSURFACE9 m_lpTempSurface = nullptr;
};
