#include "engine.hh"
#include "mesh.hh"
#include "exceptions.hh"

#include <iostream>

void Mesh::OnDeviceLost() {
	if (m_lpMesh) {
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
			if (m_lpTextures[i]) m_lpTextures[i]->Release();

		m_lpMesh->Release();
		m_lpMesh = nullptr;
		delete m_lpMaterials;
		delete m_lpTextures;
	}
}

void Mesh::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();

	/* TODO: Animations ??? */
	auto path = m_FilePath;
	auto ext = path.find_last_of('.');
	if (path.find("ani", ext))
		path.replace(ext, 4, ".x");

	DWORD size = 0;
	auto file = virtfs->Open(path, nullptr, &size);
	if (file->is_open()) {
		CHAR *mem = new CHAR[size];
		file->read(mem, size);
		virtfs->Close(file);

		LPD3DXBUFFER matbuf;
		DASSERT(D3DXLoadMeshFromXInMemory(mem, size, D3DXMESH_SYSTEMMEM,
		device, nullptr, &matbuf, nullptr,
		&m_dwNumMaterials, &m_lpMesh));
		delete mem;

		LPD3DXVECTOR3 vbuf;
		if (m_lpMesh->LockVertexBuffer(0, (LPVOID *)&vbuf) == D3D_OK) {
			D3DXComputeBoundingBox(vbuf, m_lpMesh->GetNumVertices(),
			m_lpMesh->GetNumBytesPerVertex(), &m_vBoundMin, &m_vBoundMax);
			m_lpMesh->UnlockVertexBuffer();
		}

		auto mats = (LPD3DXMATERIAL)matbuf->GetBufferPointer();
		m_lpTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
		m_lpMaterials = new D3DMATERIAL9[m_dwNumMaterials];

		for (DWORD i = 0; i < m_dwNumMaterials; i++) {
			auto mat = &mats[i];
			auto tname = mat->pTextureFilename;
			m_lpMaterials[i] = mat->MatD3D;

			if (!tname || *tname == '\0') {
				m_lpTextures[i] = nullptr;
				continue;
			}

			file = virtfs->Open(
				"maps" + std::string{std::strrchr(tname, '\\'), std::strrchr(tname, '.')} + ".dds", nullptr, &size
			);

			if (file->is_open()) {
				mem = new CHAR[size];
				file->read(mem, size);
				virtfs->Close(file);
				DASSERT(D3DXCreateTextureFromFileInMemory(device, mem, size, &(m_lpTextures[i])));
				delete mem;
				continue;
			}
		}

		matbuf->Release();
	}
}

void Mesh::Draw(LPDIRECT3DDEVICE9 device, bool untextured) {
	if (!m_lpMesh) return;

	for (DWORD i = 0; i < m_dwNumMaterials; i++) {
		if (!untextured) {
			device->SetMaterial(&m_lpMaterials[i]);
			device->SetTexture(0, m_lpTextures[i]);
		}
		m_lpMesh->DrawSubset(i);
	}
}
