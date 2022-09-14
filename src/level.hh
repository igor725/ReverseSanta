#pragma once

#include <Windows.h>
#include <vector>
#include <iterator>

#include "elems.hh"
#include "mesh.hh"

class Level {
public:
	struct Object {
		CHAR f_name[32];
		D3DXVECTOR3 f_pos[2];
		DWORD f_rot;
	};

	struct DObject {
		bool f_alerted = true;
		Mesh *f_mesh = nullptr;
		D3DXVECTOR3 f_pos = {0.0f, 0.0f, 0.0f},
		f_rot = {0.0f, 0.0f, 0.0f};
		D3DXMATRIX f_world = {};
		FLOAT f_scale = 1.0f;

		DObject() {};
		DObject(Mesh *mesh, D3DXVECTOR3 pos, D3DXVECTOR3 rot, FLOAT scale)
		: f_mesh(mesh), f_pos(pos), f_rot(rot), f_scale(scale) {}

		void Update() {
			D3DXMATRIX tm, rot, scale;

			D3DXMatrixTranslation(&f_world, f_pos.x, f_pos.y, f_pos.z);
			D3DXMatrixIdentity(&rot);

			D3DXMatrixRotationX(&tm, f_rot.x);
			D3DXMatrixMultiply(&rot, &rot, &tm);

			D3DXMatrixRotationY(&tm, f_rot.y);
			D3DXMatrixMultiply(&rot, &rot, &tm);

			D3DXMatrixRotationZ(&tm, f_rot.z);
			D3DXMatrixMultiply(&rot, &rot, &tm);

			D3DXMatrixMultiply(&f_world, &rot, &f_world);

			D3DXMatrixScaling(&scale, f_scale, f_scale, f_scale);
			D3DXMatrixMultiply(&f_world, &scale, &f_world);
		}

		void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false) {
			if (f_alerted) {
				Update();
				f_alerted = false;
			}

			device->SetTransform(D3DTS_WORLD, &f_world);
			if (f_mesh) f_mesh->Draw(device, untextured);
		}
	};

	Level();
	~Level();

	bool Load(std::string path);
	void RefreshDrawer();
	void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false);

private:
	Elems *m_lpElems = nullptr;
	DWORD m_dwObjectCount = 0;
	Object *m_lpObjects = nullptr;
	DObject *m_lpDObjects = nullptr;
	DObject *m_lpSkyBox = nullptr;
};
