#include "engine.hh"
#include "level.hh"
#include "exceptions.hh"

VOID Level::Cleanup() {
	for (DWORD i = 0; i < m_dwObjectCount; i++)
		if (auto ud = m_lpDObjects[i].f_lpUserData)
			delete ud;

	delete m_lpLObjects;
	delete m_lpDObjects;
	m_lpLObjects = nullptr;
	m_lpDObjects = nullptr;
	m_dwObjectCount = 0;
}

BOOL Level::Load(std::string path) {
	Level::Cleanup();
	DWORD fsize = 0;
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	auto cache = engine->SysCache();
	auto device = engine->SysGraphics()->GetDevice();
	DASSERT(D3DXCreateTexture(device, 16, 16, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET,
	D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTempTexture));
	DASSERT(m_lpTempTexture->GetSurfaceLevel(0, &m_lpTempSurface));
	if (auto file = virtfs->Open(path, &fsize)) {
		if (fsize < 4) {
			virtfs->Close(file);
			return false;
		}
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
			Rebuild();
		}
		virtfs->Close(file);

		if (!m_lpSkyBox) {
			m_lpSkyBox = new DObject(
				cache->GetMesh("gfx\\himmel.x"),
				{0.0f, -100.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				10.0f
			);
		}

		return true;
	}

	return false;
}

VOID Level::Rebuild() {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpLObjects[i];
		auto dobj = &m_lpDObjects[i];

		dobj->f_bAlerted = true;
		dobj->f_vPos = obj->f_vPos[0];
		dobj->f_lpElem = m_Elems.Search(obj->f_name);
		dobj->f_lpMesh = dobj->f_lpElem->f_lpMesh;
		dobj->f_vRot.y = D3DXToRadian(dobj->f_lpElem->f_fRotation + obj->f_dwRot * 90.0f);
		if (dobj->f_lpUserData) delete dobj->f_lpUserData;

		switch (auto type = dobj->f_lpElem->f_eType) {
			case Elems::MOVER:
			case Elems::ELEVATOR:
				dobj->f_lpUserData = new ElevatorData(type);
				break;
		}
	}
}

VOID Level::Update(FLOAT delta) {
	/* TODO: Логика уровня - лифты, враги, движущиеся платформы и прочая срань */
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpDObjects[i];
		auto lobj = &m_lpLObjects[i];

		switch (obj->f_lpElem->f_eType) {
			case Elems::MOVER:
			case Elems::ELEVATOR:
				((ElevatorData *)obj->f_lpUserData)->Update(obj, lobj, delta);
				break;
		}
	}
}

VOID Level::Draw(LPDIRECT3DDEVICE9 device, Camera *camera, BOOL untextured) {
	if (m_dwObjectCount > 0) {
		if (!untextured) m_lpSkyBox->Draw(device);
		for (DWORD i = 0; i < m_dwObjectCount; i++) {
			if (camera && !camera->IsVisible(m_lpDObjects[i].f_vPos))
				continue;
			if (untextured) {
				device->ColorFill(m_lpTempSurface, nullptr, i);
				device->SetTexture(0, m_lpTempTexture);
			}
			m_lpDObjects[i].Draw(device, untextured);
		}
	}
}

BOOL Level::IterTouches(DObject *obj, BOOL(*callback)(DObject *first, DObject *second, FLOAT floor, LPVOID ud), LPVOID ud) {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto second = &m_lpDObjects[i]; FLOAT floor;
		if (second->IsTouching(obj, &floor) && callback(obj, second, floor, ud))
			return true;
	}

	return false;
}

BOOL Level::IterObjects(BOOL(*callback)(ObjectData data, LPVOID ud), LPVOID ud) {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		if (callback({&m_lpDObjects[i], &m_lpLObjects[i]}, ud))
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
