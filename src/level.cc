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
	if (m_lpLObjects) delete m_lpLObjects;
	if (m_lpDObjects) delete m_lpDObjects;
}

void Level::OnDeviceLost() {
	if (m_lpSkyBox)
		m_lpSkyBox->f_lpMesh->OnDeviceLost();
	m_lpElems->OnDeviceLost();
	Refresh();
}

void Level::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	if (m_lpSkyBox)
		m_lpSkyBox->f_lpMesh->OnDeviceReset(device);
	m_lpElems->OnDeviceReset(device);
}

BOOL Level::Load(std::string path) {
	if (m_lpLObjects) {
		delete m_lpLObjects;
		m_lpLObjects = nullptr;
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
			auto bsize = m_dwObjectCount * sizeof(LObject);
			if ((bsize - fsize) < 3) {
				virtfs->Close(file);
				return false;
			}

			m_lpLObjects = new LObject[m_dwObjectCount];
			file->read((char *)m_lpLObjects, bsize);
			m_lpDObjects = new DObject[m_dwObjectCount];
			Refresh();
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

void Level::Refresh() {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpLObjects[i];
		auto dobj = &m_lpDObjects[i];

		dobj->f_bAlerted = true;
		dobj->f_vPos = obj->f_vPos[0];
		dobj->f_lpElem = m_lpElems->Search(obj->f_name);
		dobj->f_lpMesh = dobj->f_lpElem->f_lpMesh;
		dobj->f_vRot.y = D3DXToRadian(dobj->f_lpElem->f_fRotation + obj->f_dwRot * 90.0f);
	}
}

void Level::Update(FLOAT delta) {(void)delta;
	/* TODO: Логика уровня - лифты, враги, движущиеся платформы и прочая срань */
}

void Level::Draw(LPDIRECT3DDEVICE9 device, BOOL untextured) {
	if (m_dwObjectCount > 0) {
		if (!untextured) m_lpSkyBox->Draw(device);
		for (DWORD i = 0; i < m_dwObjectCount; i++) {
			// D3DXVECTOR3 v = campos - obj.f_vPos;
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

BOOL Level::IterTouches(DObject *obj, BOOL(*callback)(DObject *first, DObject *second, FLOAT floor, void *ud), void *ud) {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto second = &m_lpDObjects[i]; FLOAT floor;
		if (second->IsTouching(obj, &floor) && callback(obj, second, floor, ud))
			return true;
	}

	return false;
}

BOOL Level::GetObjectData(DWORD id, ObjectData *data) {
	if (id < m_dwObjectCount) {
		data->f_lpDObj = &m_lpDObjects[id];
		data->f_lpLObj = &m_lpLObjects[id];
		return true;
	}

	return false;
}
