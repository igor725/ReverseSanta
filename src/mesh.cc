#include "engine.hh"
#include "mesh.hh"
#include "exceptions.hh"

Mesh::Mesh(LPDIRECT3DDEVICE9 device, const std::string &path) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	auto cache = engine->SysCache();
	/* TODO: Animations ??? */
	auto _path = std::string(path);
	auto ext = _path.find_last_of('.');
	if (_path.find("ani", ext))
		_path.replace(ext, 4, ".x");
	DWORD size = 0;
	auto file = virtfs->Open(_path, nullptr, &size);
	EASSERT(file->is_open());

	auto mem = new CHAR[size];
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
	m_lpTextures = new Texture *[m_dwNumMaterials];
	m_lpMaterials = new D3DMATERIAL9[m_dwNumMaterials];

	for (DWORD i = 0; i < m_dwNumMaterials; i++) {
		auto mat = &mats[i];
		auto tname = mat->pTextureFilename;
		m_lpMaterials[i] = mat->MatD3D;

		if (!tname || *tname == '\0') {
			m_lpTextures[i] = nullptr;
			continue;
		}

		m_lpTextures[i] = cache->GetTexture(
			"maps" + std::string{std::strrchr(tname, '\\'), std::strrchr(tname, '.')} + ".dds"
		);
	}

	matbuf->Release();
}

Mesh::~Mesh() {
	m_lpMesh->Release();
	delete m_lpTextures;
	delete m_lpMaterials;
}

void Mesh::Draw(LPDIRECT3DDEVICE9 device, BOOL untextured) {
	if (!m_lpMesh) return;

	for (DWORD i = 0; i < m_dwNumMaterials; i++) {
		if (!untextured) {
			device->SetMaterial(&m_lpMaterials[i]);
			device->SetTexture(0, LPDIRECT3DTEXTURE9(*m_lpTextures[i]));
		}
		m_lpMesh->DrawSubset(i);
	}
}
