#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

#include "texture.hh"
#include "graphics.hh"
#include "virtfs.hh"

class Mesh {
public:
	Mesh(LPDIRECT3DDEVICE9 device, const std::string &path);
	~Mesh();

	inline D3DXVECTOR3 GetBoundMin(D3DXVECTOR3 pos, FLOAT scale) { return (m_vBoundMin * scale) + pos; }
	inline D3DXVECTOR3 GetBoundMax(D3DXVECTOR3 pos, FLOAT scale) { return (m_vBoundMax * scale) + pos; }
	inline D3DXVECTOR3 GetBoundCenter(D3DXVECTOR3 pos, FLOAT scale) { return (m_vBoundCenter * scale) + pos; }
	inline FLOAT GetBoundRadius(FLOAT scale) { return m_vBoundRadius * scale; }

	VOID SetBoundMin(D3DXVECTOR3 min) { m_vBoundMin = min; }
	VOID SetBoundMax(D3DXVECTOR3 max) { m_vBoundMax = max; }

	VOID Draw(LPDIRECT3DDEVICE9 device, BOOL untextured = false);

private:
	LPD3DXMESH m_lpMesh = nullptr;
	DWORD m_dwNumMaterials = 0;
	D3DMATERIAL9 *m_lpMaterials = nullptr;
	Texture **m_lpTextures = nullptr;

	D3DXVECTOR3 m_vBoundMin = {0.0f, 0.0f, 0.0f},
	m_vBoundMax = {0.0f, 0.0f, 0.0f},
	m_vBoundCenter = {0.0f, 0.0f, 0.0f};
	FLOAT m_vBoundRadius = 0.0f;
};
