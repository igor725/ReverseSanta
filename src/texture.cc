#include "engine.hh"
#include "texture.hh"
#include "exceptions.hh"

Texture::~Texture() {
	m_lpTexture->Release();
	m_lpTexture = nullptr;
}

void Texture::OnDeviceLost() {
	Texture::~Texture();
}

void Texture::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	DWORD size = 0;
	auto file = virtfs->Open(m_sFilePath, nullptr, &size);
	EASSERT(file->is_open());

	auto mem = new CHAR[size];
	file->read(mem, size);
	virtfs->Close(file);
	DASSERT(D3DXCreateTextureFromFileInMemory(device, mem, size, &m_lpTexture));
	delete mem;
}
