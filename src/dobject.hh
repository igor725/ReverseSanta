#pragma once

#include <Windows.h>
#include <d3dx9.h>

#include "elems.hh"
#include "mesh.hh"

struct DObject {
	bool f_bAlerted = true;
	Elems::Type f_eType = Elems::NONE;
	Mesh *f_lpMesh = nullptr;
	D3DXVECTOR3 f_vPos = {0.0f, 0.0f, 0.0f},
	f_vRot = {0.0f, 0.0f, 0.0f};
	D3DXMATRIX f_world = {};
	FLOAT f_scale = 1.0f;

	DObject() {};
	DObject(Mesh *mesh, D3DXVECTOR3 pos, D3DXVECTOR3 rot, FLOAT scale)
	: f_lpMesh(mesh), f_vPos(pos), f_vRot(rot), f_scale(scale) {}

	void Update() {
		D3DXMATRIX tm, rot, scale;

		D3DXMatrixTranslation(&f_world, f_vPos.x, f_vPos.y, f_vPos.z);
		D3DXMatrixIdentity(&rot);

		D3DXMatrixRotationX(&tm, f_vRot.x);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixRotationY(&tm, f_vRot.y);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixRotationZ(&tm, f_vRot.z);
		D3DXMatrixMultiply(&rot, &rot, &tm);

		D3DXMatrixMultiply(&f_world, &rot, &f_world);

		D3DXMatrixScaling(&scale, f_scale, f_scale, f_scale);
		D3DXMatrixMultiply(&f_world, &scale, &f_world);
	}

	bool IsTouching(DObject *other, FLOAT *ground) {
		auto omesh = other->f_lpMesh;
		auto max1 = f_lpMesh->GetBoundMax(f_vPos, f_scale),
		min1 = f_lpMesh->GetBoundMin(f_vPos, f_scale),
		max2 = omesh->GetBoundMax(other->f_vPos, other->f_scale),
		min2 = omesh->GetBoundMin(other->f_vPos, other->f_scale);

		if ((min1.x <= max2.x) && (max1.x >= min2.x) &&
		(min1.y <= max2.y) && (max1.y >= min2.y) &&
		(min1.z <= max2.z) && (max1.z >= min2.z)) {
			*ground = max1.y;
			return true;
		}

		return false;
	}

	void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false) {
		if (f_bAlerted) {
			Update();
			f_bAlerted = false;
		}

		device->SetTransform(D3DTS_WORLD, &f_world);
		if (f_lpMesh) f_lpMesh->Draw(device, untextured);
	}
};
