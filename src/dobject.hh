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

	void Update() {
		D3DXMATRIX tm, rot, scale;

		D3DXMatrixTranslation(&f_mxWorld, f_vPos.x, f_vPos.y, f_vPos.z);
		D3DXMatrixIdentity(&rot);

		D3DXMatrixRotationX(&tm, f_vRot.x);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixRotationY(&tm, f_vRot.y);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixRotationZ(&tm, f_vRot.z);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixMultiply(&f_mxWorld, &rot, &f_mxWorld);

		D3DXMatrixScaling(&scale, GetScale(), GetScale(), GetScale());
		D3DXMatrixMultiply(&f_mxWorld, &scale, &f_mxWorld);
	}

	/* TODO: Collision face detection */
	BOOL IsTouching(DObject *other, FLOAT *floor) {
		if (f_bHidden || other->f_bHidden) return false;
		auto omesh = other->f_lpMesh;
		auto max1 = f_lpMesh->GetBoundMax(f_vPos, GetScale()),
		min1 = f_lpMesh->GetBoundMin(f_vPos, GetScale()),
		max2 = omesh->GetBoundMax(other->f_vPos, other->GetScale()),
		min2 = omesh->GetBoundMin(other->f_vPos, other->GetScale());

		if ((min1.x <= max2.x) && (max1.x >= min2.x) &&
		(min1.y <= max2.y) && (max1.y >= min2.y) &&
		(min1.z <= max2.z) && (max1.z >= min2.z)) {
			*floor = max1.y;
			return true;
		}

		return false;
	}

	void Draw(LPDIRECT3DDEVICE9 device, BOOL untextured = false) {
		if (f_bHidden) return;
		if (f_bAlerted) {
			Update();
			f_bAlerted = false;
		}

		device->SetTransform(D3DTS_WORLD, &f_mxWorld);
		if (f_lpMesh) f_lpMesh->Draw(device, untextured);
	}
};
