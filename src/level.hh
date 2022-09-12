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
		bool f_alerted;
		Mesh *f_mesh;
		D3DXVECTOR3 f_pos, f_rot;
		D3DXMATRIX f_world;
		FLOAT f_scale;

		DObject() {}

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
	};

	Level();
	~Level();

	bool Load(std::string path);
	void RefreshDrawer();
	void Draw(LPDIRECT3DDEVICE9 device);

private:
	Elems *m_lpElems = nullptr;
	DWORD m_dwObjectCount = 0;
	Object *m_lpObjects = nullptr;
	DObject *m_lpDObjects = nullptr;
};
