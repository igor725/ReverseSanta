#include "engine.hh"
#include "rescache.hh"
#include "exceptions.hh"

#define DEVICE Engine::GetInstance()->SysGraphics()->GetDevice()

ResCache::~ResCache() {
	for (auto &f : m_mCache) {
		if (f.second.f_eType == MESH)
			delete (Mesh *)f.second;
		else
			delete (Texture *)f.second;
	}
}

Mesh *ResCache::GetMesh(const std::string &path) {
	if (m_mCache.find(path) != m_mCache.end())
		return (Mesh *)m_mCache[path];

	return (Mesh *)(m_mCache[path] = {MESH, new Mesh(DEVICE, path)});
}

Texture *ResCache::GetTexture(const std::string &path) {
	if (m_mCache.find(path) != m_mCache.end())
		return (Texture *)m_mCache[path];

	return (Texture *)(m_mCache[path] = {TEXTURE, new Texture(DEVICE, path)});
}

VOID ResCache::OnDeviceLost() {
	for (auto &f : m_mCache)
		if (f.second.f_eType == TEXTURE)
			((Texture *)f.second)->OnDeviceLost();
}

VOID ResCache::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	for (auto &f : m_mCache)
		if (f.second.f_eType == TEXTURE)
			((Texture *)f.second)->OnDeviceReset(device);
}
