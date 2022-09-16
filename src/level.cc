#include "engine.hh"
#include "level.hh"
#include "exceptions.hh"

Level::Level() {
	auto virtfs = Engine::GetInstance()->SysVirtFs();
	DWORD size;
	std::ifstream *file = virtfs->Open("data\\elements.txt", &size);
	if (file->is_open()) {
		m_lpElems = new Elems(file, size);
		virtfs->Close(file);
	}
}

Level::~Level() {
	delete m_lpElems;
	if (m_lpObjects) delete m_lpObjects;
	if (m_lpDObjects) delete m_lpDObjects;
}

void Level::OnDeviceLost() {
	if (m_lpSkyBox)
		m_lpSkyBox->f_mesh->OnDeviceLost();
	m_lpElems->OnDeviceLost();
	RefreshDrawer();
}

void Level::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	if (m_lpSkyBox)
		m_lpSkyBox->f_mesh->OnDeviceReset(device);
	m_lpElems->OnDeviceReset(device);
}

bool Level::Load(std::string path) {
	if (m_lpObjects) {
		delete m_lpObjects;
		m_lpObjects = nullptr;
	}
	if (m_lpDObjects) {
		delete m_lpDObjects;
		m_lpDObjects = nullptr;
	}
	m_dwObjectCount = 0;

	DWORD fsize = 0;
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	auto device = engine->SysGraphics()->GetDevice();
	DASSERT(D3DXCreateTexture(device, 32, 32, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET,
	D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTempTexture));
	DASSERT(m_lpTempTexture->GetSurfaceLevel(0, &m_lpTempSurface));
	auto file = virtfs->Open(path, &fsize);
	if (file->is_open() && fsize > 4) {
		file->read((char *)&m_dwObjectCount, 4);
		if (m_dwObjectCount > 0) {
			auto bsize = m_dwObjectCount * sizeof(Object);
			if ((bsize - fsize) < 3) {
				virtfs->Close(file);
				return false;
			}

			m_lpObjects = new Object[m_dwObjectCount];
			file->read((char *)m_lpObjects, bsize);
			m_lpDObjects = new DObject[m_dwObjectCount];
			RefreshDrawer();
		}
		virtfs->Close(file);

		if (!m_lpSkyBox) {
			m_lpSkyBox = new DObject(
				new Mesh(device, "gfx\\himmel.x"),
				{0.0f, -100.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				10.0f
			);
		}

		return true;
	}

	return false;
}

void Level::RefreshDrawer() {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpObjects[i];
		auto dobj = &m_lpDObjects[i];
		auto elem = m_lpElems->Search(obj->f_name);

		dobj->f_alerted = true;
		dobj->f_pos = obj->f_pos[0];
		dobj->f_mesh = elem->u_mesh;
		dobj->f_scale = elem->f_scaling / 100.0f;
		dobj->f_rot.y = D3DXToRadian(elem->f_rotation + obj->f_rot * 90.0f);
	}
}

void Level::Draw(LPDIRECT3DDEVICE9 device, bool untextured) {
	if (m_dwObjectCount > 0) {
		if (!untextured) m_lpSkyBox->Draw(device);
		for (DWORD i = 0; i < m_dwObjectCount; i++) {
			// D3DXVECTOR3 v = campos - obj.f_pos;
			// if (D3DXVec3Length(&v) < 1000.f) {
				if (untextured) {
					device->ColorFill(m_lpTempSurface, nullptr, i);
					device->SetTexture(0, m_lpTempTexture);
				}
				m_lpDObjects[i].Draw(device, untextured);
			// }
		}
	}
}
