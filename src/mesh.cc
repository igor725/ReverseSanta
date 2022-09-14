#include "engine.hh"
#include "mesh.hh"
#include "exceptions.hh"

Mesh::Mesh(LPDIRECT3DDEVICE9 device, std::string path) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();

	DWORD size = 0;
	auto file = virtfs->Open(path, nullptr, &size);
	if (file->is_open()) {
		CHAR *mem = new CHAR[size];
		file->read(mem, size);
		virtfs->Close(file);

		DASSERT(D3DXLoadMeshFromXInMemory(mem, size, D3DXMESH_SYSTEMMEM,
		device, nullptr, &m_lpMatBuffer, nullptr,
		&m_dwNumMaterials, &m_lpMesh));
		delete mem;

		auto mats = (LPD3DXMATERIAL)m_lpMatBuffer->GetBufferPointer();
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
	}
}

Mesh::~Mesh() {
	m_lpMesh->Release();
	delete m_lpMaterials;
	delete m_lpTextures;
}

void Mesh::Draw(LPDIRECT3DDEVICE9 device, bool untextured) {
	for (DWORD i = 0; i < m_dwNumMaterials; i++) {
		DASSERT(device->SetMaterial(&m_lpMaterials[i]));
		if (!untextured) { DASSERT(device->SetTexture(0, m_lpTextures[i])); }
		DASSERT(m_lpMesh->DrawSubset(i));
	}
}
