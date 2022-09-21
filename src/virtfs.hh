#pragma once

#include "xpk.hh"

class VirtFs {
public:
	VirtFs(std::string xpk) : m_lpXpk(xpk) {}

	std::ifstream *Open(std::string &path, DWORD *end = nullptr, DWORD *size = nullptr);
	void Close(std::ifstream *file);

private:
	Xpk m_lpXpk;
};
