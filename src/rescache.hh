#pragma once

#include "exceptions.hh"
#include "texture.hh"
#include "mesh.hh"
#include <string>
#include <map>

class ResCache {
public:
	~ResCache();

	Mesh *GetMesh(const std::string &path);
	Texture *GetTexture(const std::string &path);

	VOID OnDeviceLost();
	VOID OnDeviceReset(LPDIRECT3DDEVICE9 device);

private:
	enum Type {
		INVALID,
		MESH,
		TEXTURE
	};

	struct Field {
		Type f_eType;
		LPVOID f_lpData;

		operator Mesh *() {
			EASSERT(f_eType == MESH);
			return (Mesh *)f_lpData;
		}

		operator Texture *() {
			EASSERT(f_eType == TEXTURE);
			return (Texture *)f_lpData;
		}
	};

	std::map<std::string, struct Field>m_mCache = {};
};
