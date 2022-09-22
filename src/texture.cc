#include "engine.hh"
#include "texture.hh"
#include "exceptions.hh"

VOID Texture::OnDeviceLost() {
	m_lpTexture->Release();
	m_lpTexture = nullptr;
}

VOID Texture::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	DWORD size = 0;

	if (auto file = virtfs->Open(m_sFilePath, nullptr, &size)) {
		auto mem = new CHAR[size];
		file->read(mem, size);
		virtfs->Close(file);
		DASSERT(D3DXCreateTextureFromFileInMemory(device, mem, size, &m_lpTexture));
		delete mem;
		return;
	}

	EASSERT(0 && "Failed to load texture");
}
