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

	inline D3DXVECTOR3 GetBoundMin(D3DXVECTOR3 pos, FLOAT scale) { return (m_vBoundMin * scale) + pos; }
	inline D3DXVECTOR3 GetBoundMax(D3DXVECTOR3 pos, FLOAT scale) { return (m_vBoundMax * scale) + pos; }

	void SetBoundMin(D3DXVECTOR3 min) { m_vBoundMin = min; }
	void SetBoundMax(D3DXVECTOR3 max) { m_vBoundMax = max; }

	void Draw(LPDIRECT3DDEVICE9 device, bool untextured = false);

private:
	std::string m_FilePath;

	LPD3DXMESH m_lpMesh = nullptr;
	DWORD m_dwNumMaterials = 0;
	D3DMATERIAL9 *m_lpMaterials = nullptr;
	LPDIRECT3DTEXTURE9 *m_lpTextures = nullptr;

	D3DXVECTOR3 m_vBoundMin = {0.0f, 0.0f, 0.0f},
	m_vBoundMax = {0.0f, 0.0f, 0.0f};
};
