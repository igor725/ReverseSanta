#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "elems.hh"
#include "mesh.hh"

struct DObject {
	BOOL f_bHidden = false,
	f_bAlerted = true;
	Elems::Element *f_lpElem = nullptr;
	Mesh *f_lpMesh = nullptr;
	D3DXVECTOR3 f_vPos = {0.0f, 0.0f, 0.0f},
	f_vRot = {0.0f, 0.0f, 0.0f};
	D3DXMATRIX f_mxWorld = {};
	FLOAT f_fScaling = 1.0f;
	LPVOID f_lpUserData = nullptr;

	DObject() {}
	DObject(Mesh *mesh, D3DXVECTOR3 pos, D3DXVECTOR3 rot, FLOAT scale = 1.0f)
	: f_lpMesh(mesh), f_vPos(pos), f_vRot(rot), f_fScaling(scale) {}

	inline FLOAT GetScale() { return f_lpElem ? f_lpElem->f_fScaling : f_fScaling; }

	BOOL IsTouching(LPD3DXVECTOR3 p);
	BOOL IsTouching(DObject *other, FLOAT *floor);

	VOID Update();
	VOID Draw(LPDIRECT3DDEVICE9 device, BOOL untextured = false);
};
