#include "engine.hh"
#include "level.hh"
#include "exceptions.hh"
#include "todo.hh"

VOID Level::ElevatorData::Update(DObject *dobj, LObject *lobj, FLOAT delta) {
	if (f_dwState < 1) return;
	dobj->f_bAlerted = true;
	auto prmult = f_fMult;
	f_fMult = (0.5f * (1.0f + std::sinf(2 * D3DX_PI * f_fTime)));
	if (f_bStopOnFinish) {
		if (f_dwState % 2 ? f_fMult < prmult : f_fMult > prmult) f_dwState++;
		if (f_dwState == 3) {
			f_vMove = {0.0f, 0.0f, 0.0f};
			f_fTime = 0.75f;
			f_dwState = 0;
			f_fMult = 0;
			return;
		}
	}
	f_vMove = dobj->f_vPos;
	auto dir = lobj->f_vPos[1] - lobj->f_vPos[0];
	dobj->f_vPos = lobj->f_vPos[0] + dir * f_fMult;
	f_vMove -= dobj->f_vPos;
	auto len = D3DXVec3Length(&dir);
	f_fTime += delta / (len / 1.5f);
}

BOOL Level::EnemyData::CheckForward(Level *level, DObject *me) {
	struct ForwardState {
		BOOL ground;
		DObject *me;
		D3DXVECTOR3 fwdpt;
	} fst = {
		false, me,
		me->f_vPos + D3DXVECTOR3{f_fFwdX, 0.04f, f_fFwdZ}
	};

	return !level->IterObjects([](Level::ObjectData data, LPVOID ud)->BOOL {
		auto fst = (struct ForwardState *)ud;
		auto dobj = data.f_lpDObj;

		if (dobj == fst->me) return false;
		if (dobj->IsTouching(&fst->fwdpt) && dobj->f_lpElem->f_eType != Elems::ENEMY) return true;
		if (fst->ground) return false;

		fst->fwdpt.y -= 0.06f;
		fst->ground = dobj->IsTouching(&fst->fwdpt);
		fst->fwdpt.y += 0.06f;
		return false;
	}, &fst) && fst.ground;
}

VOID Level::EnemyData::TrollTick(Level *level, DObject *me, FLOAT delta) {
	TODO("Optimize this shit");
	if (!f_bEnabled) return;
	if (f_dwState == 0) {
		auto &rot = me->f_vRot.y;
		for (FLOAT r = 0.0f; r <= 2 * D3DX_PI; r += D3DX_PI * 0.5f) {
			f_fFwdX = std::sinf(rot + r) * 1.5f, f_fFwdZ = std::cosf(rot + r) * 1.5f;
			if (!CheckForward(level, me)) continue;
			me->f_bAlerted = true;
			f_dwState = 1;
			if ((rot += r) >= 2.0f * D3DX_PI)
				rot = 0.0f;
			break;
		}
	} else if (f_dwState == 1) {
		auto speed = delta * 3.5f;
		if (!CheckForward(level, me)) f_dwState = 0;
		me->f_vPos.x += f_fFwdX * speed;
		me->f_vPos.z += f_fFwdZ * speed;
		me->f_bAlerted = true;
	}
}

VOID Level::EnemyData::Update(Level *level, DObject *self, FLOAT delta) {
	switch (f_dwBehaviourHash) {
		case 0xC9C7FF44: // RABE
		case 0x508F15A5: // RABE BIG
			CrowTick(level, self, delta);
			break;
		case 0xFA9BBD57: // RABE ELEVATOR
		case 0x6BBA4D7C: // RABE ELEVATOR BIG RADIUS
			break;

		case 0x564B74AA: // SCHNEEMANN
			SnowmanTick(level, self, delta);
			break;
		case 0x2AAD003B: // SCHNEEMANN ELEVATOR
			break;

		case 0x35A53520: // TROLL
		case 0xDC08B1CE: // TROLL BIG
			TrollTick(level, self, delta);
			break;
		case 0xE06BF2D3: // TROLL ELEVATOR
			break;

		case 0xA0A5A3CE: // Rectform FIRE
			// Огонёк слегка тупой и не имеет искусственного интелекта, так что тут пусто
			break;

		default: EASSERT(0 && "Unknown enemy type");
	}
}

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
			case Elems::ENEMY:
			case Elems::ELEVATORENEMY:
				dobj->f_lpUserData = new EnemyData(dobj->f_lpElem->f_dwHash);
				break;
		}
	}
}

VOID Level::Update(FLOAT delta) {
	static DWORD counter = 0;
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpDObjects[i];
		auto lobj = &m_lpLObjects[i];

		switch (obj->f_lpElem->f_eType) {
			case Elems::MOVER:
			case Elems::ELEVATOR:
				((ElevatorData *)obj->f_lpUserData)->Update(obj, lobj, delta);
				break;
			case Elems::ENEMY:
			case Elems::ELEVATORENEMY:
				if (counter == 0)
					((EnemyData *)obj->f_lpUserData)->Update(this, obj, delta * 6);
				break;
		}
	}
	counter = ++counter % 6;
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

BOOL Level::IsTouchingAnything(LPD3DXVECTOR3 point, DObject *except) {
	for (DWORD i = 0; i < m_dwObjectCount; i++) {
		auto obj = &m_lpDObjects[i];
		if (obj != except && obj->IsTouching(point)) return true;
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
