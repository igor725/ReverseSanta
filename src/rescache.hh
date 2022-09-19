#pragma once

#include "exceptions.hh"
#include "texture.hh"
#include "mesh.hh"
#include <string>
#include <map>

class ResCache {
public:
	Mesh *GetMesh(const std::string &path);
	Texture *GetTexture(const std::string &path);

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

private:
	enum Type {
		MESH,
		TEXTURE
	};

	struct Field {
		Type f_eType;
		void *f_lpData;

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
