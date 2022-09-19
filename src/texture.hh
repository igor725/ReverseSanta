#pragma once

#include <d3d9.h>
#include <string>

class Texture {
public:
	Texture(LPDIRECT3DDEVICE9 device, const std::string &path) : m_sFilePath(path) { OnDeviceReset(device); }
	~Texture();

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);
	inline operator LPDIRECT3DTEXTURE9() const {
		return this ? m_lpTexture : nullptr;
	}

private:
	std::string m_sFilePath;
	LPDIRECT3DTEXTURE9 m_lpTexture = nullptr;
};
