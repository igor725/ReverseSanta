#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

#include "graphics.hh"
#include "virtfs.hh"

class Mesh {
public:
	Mesh(LPDIRECT3DDEVICE9 device, std::string path) : m_FilePath(path) { OnDeviceReset(device); }
	~Mesh() { OnDeviceLost(); }

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false);

private:
	DWORD m_dwNumMaterials;
	LPD3DXBUFFER m_lpMatBuffer;
	D3DMATERIAL9 *m_lpMaterials;
	LPDIRECT3DTEXTURE9 *m_lpTextures;
	LPD3DXMESH m_lpMesh;
	std::string m_FilePath;
};
